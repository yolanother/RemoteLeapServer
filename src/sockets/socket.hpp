#ifndef DT_SOCKET
#define DT_SOCKET

#include <vector>
#include <thread>
#include <sys/socket.h>

#include "datawriter.hpp"

namespace DoubTech {
    namespace Sockets {
        class Socket : public DataWriter {
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
                void send(const char* buffer, size_t len);
                virtual void send(std::vector<Data> data) override {
                    for(Data data : data) {
                        send(data.data(), data.length());
                    }
                }
                virtual void send(Data data) override {
                    DataWriter::send(data);
                }
            private:
                int socketFd;
                std::thread socketThread;
        };
    };
};

#endif