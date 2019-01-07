#include "server.hpp"
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

#define TAG "Server"

void Server::accept() {
    const int socketFd = getSocketFd();
    while (socketFd != INVALID_SOCKET_ID) {
        struct  sockaddr_storage    serverStorage;
        socklen_t                   addr_size   = sizeof serverStorage;
        
        logd("Waiting for client connections...");
        int newSocket = ::accept(socketFd, (struct sockaddr*)&serverStorage, &addr_size);
        if (newSocket == INVALID_SOCKET_ID) {
            // If the socket has been shutdown leave the main thread.
            if(errno == 53) break;
            throwRuntime("accept: ", strerror(errno), " [", errno, "]");
        }
        logd("Accepted new client connection");
        Socket socket = Socket(newSocket);
        sockets.push_back(std::move(socket));
    }
}

void Server::onStart() {
    acceptThread = std::thread( [=] { accept(); } );
}

void Server::onStop() {
    for(auto it = sockets.begin(); it < sockets.end(); it++) {
        it->disconnect();
    }

    sockets.clear();
    acceptThread.join();
}

int Server::onCreateSocketFd() {
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

void Server::send(std::vector<Data> data) {
    for(int i = sockets.size() - 1; i >= 0; --i) {
        if(!sockets[i].checkStatus()) {
            sockets.erase(sockets.begin() + i);
        }
        sockets[i].send(data);
    }
    for(auto it = sockets.begin(); it < sockets.end(); it++) {
        it->send(data);
    }
}