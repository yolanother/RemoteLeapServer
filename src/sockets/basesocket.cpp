#include "basesocket.hpp"
#include "../utilities/StringUtils.hpp"
#include "../utilities/logging.hpp"

#include <sys/socket.h>
#include <unistd.h>

using namespace DoubTech::Sockets;
using namespace ThorsAnvil::Socket;

#define TAG "BaseSocket"

BaseSocket::BaseSocket() : socketFd(INVALID_SOCKET_ID) {

}

BaseSocket::~BaseSocket() {
    if(socketFd != INVALID_SOCKET_ID) {
        stop();
    }
}

// Move Semantics
void BaseSocket::swap(BaseSocket& other) noexcept {
    using std::swap;
    swap(socketFd, other.socketFd);
}

BaseSocket::BaseSocket(BaseSocket&& move) noexcept
    : socketFd(INVALID_SOCKET_ID) {
    move.swap(*this);
}

BaseSocket& BaseSocket::operator=(BaseSocket&& move) noexcept {
    move.swap(*this);
    return *this;
}

void BaseSocket::start() {
    if(this->socketFd != INVALID_SOCKET_ID) {
        stop();
    }

    this->socketFd = onCreateSocketFd();
    onStart();
}

void BaseSocket::stop() {
    if(INVALID_SOCKET_ID != socketFd) {
        logd("Stopping socket...");
        close(socketFd);
        onStop();
    }
}