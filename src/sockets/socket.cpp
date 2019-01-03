#include "socket.hpp"
#include "../utilities/StringUtils.hpp"
#include "../utilities/logging.hpp"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>

using namespace DoubTech::Sockets;
using namespace ThorsAnvil::Socket;

#define TAG "Socket"

Socket::~Socket() {
    if(socketFd != INVALID_SOCKET_ID) {
        disconnect();
    }
}

// Move Semantics
void Socket::swap(Socket& other) noexcept {
    using std::swap;
    swap(socketFd, other.socketFd);
}

Socket::Socket(Socket&& move) noexcept
    : socketFd(INVALID_SOCKET_ID) {
    move.swap(*this);
}

Socket& Socket::operator=(Socket&& move) noexcept {
    move.swap(*this);
    return *this;
}

void Socket::disconnect() {
    if (::shutdown(socketFd, SHUT_WR) != 0) {
        throwRuntime("shutdown: critical error: ", strerror(errno));
    }
    socketFd = INVALID_SOCKET_ID;
}

void Socket::send(char const *buffer, int len) {
    if(INVALID_SOCKET_ID == socketFd) {
        throwRuntime("Sending data on a disconnected socket");
    }

    std::size_t     dataWritten = 0;

    while(dataWritten < len)
    {
        std::size_t put = write(socketFd, buffer + dataWritten, len - dataWritten);
        if (put == static_cast<std::size_t>(-1))
        {
            switch(errno)
            {
                case EINVAL:
                case EBADF:
                case ECONNRESET:
                case ENXIO:
                case EPIPE:
                {
                    // Fatal error. Programming bug
                    throwDomainError("write: critical error: ", strerror(errno));
                }
                case EDQUOT:
                case EFBIG:
                case EIO:
                case ENETDOWN:
                case ENETUNREACH:
                case ENOSPC:
                {
                    // Resource acquisition failure or device error
                    throwRuntime("write: resource failure: ", strerror(errno));
                }
                case EINTR:
                        // TODO: Check for user interrupt flags.
                        //       Beyond the scope of this project
                        //       so continue normal operations.
                case EAGAIN:
                {
                    // Temporary error.
                    // Simply retry the read.
                    continue;
                }
                default:
                {
                    throwRuntime("write: returned -1: ", strerror(errno));
                }
            }
        }
        dataWritten += put;
    }
}