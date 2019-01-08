#ifndef DT_SOCKET_BASE_H
#define DT_SOCKET_BASE_H

// Loosely based off of:
// https://github.com/Loki-Astari/Examples/blob/master/Version2

#include <sys/socket.h>

#include "datawriter.hpp"

namespace DoubTech {
    namespace Sockets {
        class BaseServer : public DataWriter {
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
                BaseServer(BaseServer&& move)               noexcept;
                BaseServer& operator=(BaseServer&& move)    noexcept;
                void swap(BaseServer& other)                noexcept;
                BaseServer(BaseServer const&)               = delete;
                BaseServer& operator=(BaseServer const&)    = delete;
                BaseServer();
                virtual ~BaseServer();

                void stop();
                void start();
        };
    }
}

#endif