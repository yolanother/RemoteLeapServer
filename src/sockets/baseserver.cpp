#include "baseserver.hpp"
#include "../utilities/StringUtils.hpp"
#include "../utilities/logging.hpp"

#include <sys/socket.h>
#include <unistd.h>

using namespace DoubTech::Sockets;
using namespace ThorsAnvil::Socket;

#define TAG "BaseServer"

BaseServer::BaseServer() : socketFd(INVALID_SOCKET_ID) {

}

BaseServer::~BaseServer() {
    if(socketFd != INVALID_SOCKET_ID) {
        logw("Socket was not shut down before destroying server object. ");
        close(socketFd);
        socketFd = INVALID_SOCKET_ID;
    }
}

// Move Semantics
void BaseServer::swap(BaseServer& other) noexcept {
    using std::swap;
    swap(socketFd, other.socketFd);
}

BaseServer::BaseServer(BaseServer&& move) noexcept
    : socketFd(INVALID_SOCKET_ID) {
    move.swap(*this);
}

BaseServer& BaseServer::operator=(BaseServer&& move) noexcept {
    move.swap(*this);
    return *this;
}

void BaseServer::start() {
    if(this->socketFd != INVALID_SOCKET_ID) {
        stop();
    }

    this->socketFd = onCreateSocketFd();
    onStart();
    if(isConnected()) {
        onConnected();
    }
}

void BaseServer::stop() {
    if(INVALID_SOCKET_ID != socketFd) {
        logd("Stopping socket...");
        shutdown(socketFd, SHUT_WR);
        close(socketFd);
        socketFd = INVALID_SOCKET_ID;
        onStop();
    }
}

void BaseServer::processReceivedData(std::shared_ptr<Data> data) {
    if(onReceive(data)) {
        return;
    }

    bool expired = false;
    for(auto weakListener : dataReaders) {
        if(auto listener = weakListener.lock()) {
            listener->onReceive(data);
        } else {
            expired = true;
        }
    }

    if(expired) cleanExpired();
}

void BaseServer::removeDataReader(std::weak_ptr<DataReader> reader)  {
    if (auto listener = reader.lock()) {
        auto it = std::find_if(dataReaders.begin(), dataReaders.end(), 
                [&listener](std::weak_ptr<DataReader> const& storedPtr) {
                    if(auto stored = storedPtr.lock()) {
                        return stored.get() == listener.get();
                    }

                    return false;
                });
        if(it != dataReaders.end()) {
            dataReaders.erase(it);
        }
    }
    
    cleanExpired();
}

void BaseServer::cleanExpired() {
    // Clean up any expired data readers
    for(int i = dataReaders.size(); i >= 0; i--) {
        if(dataReaders[i].expired()) {
            dataReaders.erase(dataReaders.begin() + i);
        }
    }
}
