#ifndef DT_SOCKET
#define DT_SOCKET

#include <vector>
#include <thread>
#include <sys/socket.h>

namespace DoubTech {
    namespace Sockets {
        class Socket {
            void runSocketThread();

            public:
                static const int INVALID_SOCKET_ID = -1;

                Socket(int socketHandle) : socketFd(socketHandle) {};

                // Moveable but not Copyable
                Socket(Socket&& move)               noexcept;
                Socket& operator=(Socket&& move)    noexcept;
                void swap(Socket& other)            noexcept;
                Socket(Socket const&)               = delete;
                Socket& operator=(Socket const&)    = delete;
                virtual ~Socket();

                void disconnect();
                void send(char const *buffer, int len);
            private:
                int socketFd;
                std::thread socketThread;
        };
    };
};

#endif