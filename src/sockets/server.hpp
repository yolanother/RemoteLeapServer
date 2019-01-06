#ifndef DT_SOCKET_SERVER
#define DT_SOCKET_SERVER

#include <vector>
#include <thread>
#include <sys/socket.h>

#include "socket.hpp"
#include "basesocket.hpp"
#include "datawriter.hpp"

namespace DoubTech {
    namespace Sockets {

        class Server : public BaseSocket, public DataWriter {
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
                virtual ~Server() {};
                Server(int port) : port(port) {};

                void setMaxConectionBacklog(int max) { maxConnectionBacklog = max; }
                virtual void send(std::vector<Data> data) override;
        };
    }
}

#endif