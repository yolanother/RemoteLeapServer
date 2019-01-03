#ifndef DT_SOCKET_BASE_H
#define DT_SOCKET_BASE_H

// Loosely based off of:
// https://github.com/Loki-Astari/Examples/blob/master/Version2

#include <sys/socket.h>

#include "socket.hpp"

namespace DoubTech {
    namespace Sockets {
        class BaseSocket {
            private:
                int socketFd;

            protected:
                virtual void onStop() = 0;
                virtual void onStart() = 0;
                virtual int onCreateSocketFd() = 0;

                int getSocketFd() { return socketFd; };

            public:
                static const int INVALID_SOCKET_ID = -1;
                
                // Moveable but not Copyable
                BaseSocket(BaseSocket&& move)               noexcept;
                BaseSocket& operator=(BaseSocket&& move)    noexcept;
                void swap(BaseSocket& other)                noexcept;
                BaseSocket(BaseSocket const&)               = delete;
                BaseSocket& operator=(BaseSocket const&)    = delete;
                BaseSocket();
                virtual ~BaseSocket();

                void stop();
                void start();
        };
    }
}

#endif