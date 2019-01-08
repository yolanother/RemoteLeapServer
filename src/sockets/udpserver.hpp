#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <vector>
#include <thread>
#include <sys/socket.h>

#include "socket.hpp"
#include "baseserver.hpp"
#include "datawriter.hpp"

namespace DoubTech {
    namespace Sockets {
        class UdpServer : public BaseServer {
            private:
                int maxConnectionBacklog = 5;
                int port;

            protected:
                virtual void onStart() override;
                virtual void onStop() override;
                virtual int onCreateSocketFd() override;

            public:
                virtual ~UdpServer() {};
                UdpServer(int port) : port(port) {};

                void setMaxConectionBacklog(int max) { maxConnectionBacklog = max; }
                virtual void send(Data data) override;
        };
    }
}

#endif