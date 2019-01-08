#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <vector>
#include <thread>
#include <sys/socket.h>

#include "socket.hpp"
#include "baseserver.hpp"

namespace DoubTech {
    namespace Sockets {


        class TcpServer : public BaseServer {
            private:
                int maxConnectionBacklog = 5;
                int port;
                std::vector<Socket> sockets;
                std::thread acceptThread;

                void accept();

            protected:
                virtual void onStart() override;
                virtual void onStop() override;
                virtual int onCreateSocketFd() override;

            public:
                virtual ~TcpServer() {};
                TcpServer(int port) : port(port) {};

                void setMaxConectionBacklog(int max) { maxConnectionBacklog = max; }
                virtual void send(Data data) override;
        };
    }
}

#endif