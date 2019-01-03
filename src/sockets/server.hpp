#ifndef DT_SOCKET_SERVER
#define DT_SOCKET_SERVER

#include <vector>
#include <thread>
#include <sys/socket.h>

#include "socket.hpp"
#include "basesocket.hpp"

namespace DoubTech {
    namespace Sockets {

        class Server : public BaseSocket {
            private:
                int maxConnectionBacklog = 5;
                int port;
                std::vector<Socket> sockets;
                std::thread acceptThread;

                void accept();

            protected:
                virtual void onStart();
                virtual void onStop();
                virtual int onCreateSocketFd();

            public:
                virtual ~Server() {};
                Server(int port) : port(port) {};

                void setMaxConectionBacklog(int max) { maxConnectionBacklog = max; }
                void send(const char* buffer, int len);
        };
    }
}

#endif