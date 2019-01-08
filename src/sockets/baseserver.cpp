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
}

void BaseServer::stop() {
    if(INVALID_SOCKET_ID != socketFd) {
        logd("Stopping socket...");
        close(socketFd);
        socketFd = INVALID_SOCKET_ID;
        onStop();
    }
}