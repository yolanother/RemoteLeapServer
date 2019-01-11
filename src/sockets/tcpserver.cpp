#include "tcpserver.hpp"
#include "../utilities/StringUtils.hpp"
#include "../utilities/logging.hpp"

#include "datawriter.hpp"

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <thread>

using namespace DoubTech::Sockets;
using namespace ThorsAnvil::Socket;

#define TAG "TcpServer"

void TcpServer::accept() {
    logd("Waiting for client connections...");
    while (isConnected()) {
        try {
            std::shared_ptr<TcpConnection> connection = std::make_shared<TcpConnection>(this);     
            connection->start();
            if(isConnected() && connection->isConnected()) {
                logd("Accepted new client connection");
                mutex.lock();
                connections.push_back(connection);
                mutex.unlock();
            }
        } catch (const std::exception &exc) {
            // catch anything thrown within try block that derives from std::exception
            loge(exc.what());
        } catch (...) {
            loge("Unknown error happened in the accept thread.");
        }
    }
}

void TcpServer::onStart() {
    connected = true;
    acceptThread = std::thread( [=] { accept(); } );
}

void TcpServer::onStop() {
    connected = false;
    for(auto connection : connections) {
        connection->stop();
    }

    connections.clear();
    if(acceptThread.joinable()) {
        acceptThread.join();
    }
}

int TcpServer::onCreateSocketFd() {
    logd("Creating socket...");
    int socketFd = ::socket(PF_INET, SOCK_STREAM, 0);

    if(socketFd == INVALID_SOCKET_ID) {
        throwRuntime(": Error creating socket handle.", strerror(errno));
    }
    logd("  - done");


    struct sockaddr_in serverAddr;
    bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = INADDR_ANY;

    logd("Binding to port ", port);
    if (::bind(socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0) {
        ::close(socketFd);
        throwRuntime("bind: ", strerror(errno));
    }
    logd("  - done.");

    logd("Listening for connections...");
    if (::listen(socketFd, maxConnectionBacklog) != 0) {
        ::close(socketFd);
        throwRuntime("listen: ", strerror(errno));
    }
    logd("  - done.");

    return socketFd;
}

void TcpServer::removeConnection(const TcpConnection* connection) {
    mutex.lock();
    for(auto i = connections.begin(); i < connections.end(); i++) {
        if(i->get() == connection) {
            connections.erase(i);
            break;
        }
    }
    mutex.unlock();
}

void TcpServer::send(std::shared_ptr<IData> data) {
    {
        mutex.lock();
        for(int i = connections.size() - 1; i >= 0; --i) {
            if(!connections[i]->isConnected()) {
                connections.erase(connections.begin() + i);
                logd("Removing dead connection.");
            }
        }
        mutex.unlock();
    }
    if(connections.size() > 0) logd("Sending data to ", connections.size(), " connections.");
    for(auto connection : connections) {
        connection->send(data);
    }
}

bool TcpServer::isConnected() {
    return connected && getSocketFd() != INVALID_SOCKET_ID;
}

int TcpServer::TcpConnection::onCreateSocketFd() {
    int serverSocketFd = server->getSocketFd();
    if(serverSocketFd == INVALID_SOCKET_ID) {
        throwRuntime("Server socket fd is invalid.");
    }

    struct  sockaddr_storage    serverStorage;
    socklen_t                   addr_size   = sizeof serverStorage;
    int newSocket = ::accept(serverSocketFd, (struct sockaddr*)&serverStorage, &addr_size);
    if (newSocket == INVALID_SOCKET_ID) {
        if(errno != 53) {
            throwRuntime("Could not accept connection.: ", strerror(errno), " [", errno, "]");
        }
        return INVALID_SOCKET_ID;
    }
    return newSocket;
}

bool TcpServer::TcpConnection::isConnected() {
    int socketFd = getSocketFd();
    if(socketFd == INVALID_SOCKET_ID) return false;

    int error_code;
    socklen_t error_code_size = sizeof(error_code);
    getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);
    return error_code == 0;
}

void TcpServer::TcpConnection::onStart() {
    receiveThread = std::thread( [=] { receive(); } );
}

void TcpServer::TcpConnection::onStop() {
        logd("Rejoining receive thread.");
        receiveThread.detach();
    server->removeConnection(this);
}

void TcpServer::TcpConnection::receive() {
    char buffer[bufferSize];
    try {
        int socketFd = getSocketFd();
        while(isConnected()) {
            std::shared_ptr<Data> data = std::make_shared<Data>();
            std::size_t dataRead;

            do {
                dataRead = read(socketFd, buffer, bufferSize);
                if (dataRead == static_cast<std::size_t>(-1)) {
                    switch(errno) {
                        case EAGAIN: {
                            // Temporary error.
                            // Simply retry the read.
                            continue;
                        }
                        case EPIPE: {
                            logd("Remote disconnected. Stopping");
                        }
                        default: {
                            stop();
                            return;
                        }
                    }
                }
                data->push_back(buffer, dataRead);
            } while (dataRead == bufferSize);
            if(data->size() > 0) onReceive(data);
        }
    } catch (const std::exception &exc) {
        // catch anything thrown within try block that derives from std::exception
        loge(exc.what());
    } catch (...) {
        loge("Unknwon error happened on receive thread.");
    }
    logd("No longer receiving on this connenction: ", getSocketFd());
}

bool TcpServer::TcpConnection::onReceive(std::shared_ptr<Data> data) {
    server->processReceivedData(data);
    return false;
}


void TcpServer::TcpConnection::send(std::shared_ptr<IData> data) {
    std::size_t     dataWritten = 0;
    int socketFd = getSocketFd();
    logd("Sending data to ", socketFd);
    while(dataWritten < data->size()) {
        std::size_t put = ::write(socketFd, data->data() + dataWritten, data->size() - dataWritten);
        if (errno != 0) {
            if(EPIPE == errno) {
                logd("Remote disconnected. Stopping");
                stop();
            } else {
                loge(buildErrorMessage("DataSocket::", __func__, ": write: failed to write: ", strerror(errno)));
            }
            break;
        } else {
            dataWritten += put;
            logd("Sending data: isConnected? ", isConnected(), ", dataWritten: ", dataWritten, "/", data->size());
        }
    }
    logd("Data write complete.");
}