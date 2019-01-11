#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <vector>
#include <thread>
#include <sys/socket.h>

#include "baseserver.hpp"

namespace DoubTech {
    namespace Sockets {
        class TcpServer : public BaseServer {
            class TcpConnection : public BaseServer {
                public:
                    TcpConnection(TcpServer* server) : server(server) {};
                    virtual bool isConnected() override;
                    virtual void send(std::shared_ptr<IData> data) override;
                protected:
                    virtual void onStart() override;
                    virtual void onStop() override;
                    virtual void onConnected() override {};
                    virtual int onCreateSocketFd() override;
                    virtual bool onReceive(std::shared_ptr<Data> data) override;
                private:
                    TcpServer* server;
                    std::thread receiveThread;
                    int bufferSize = 2048;

                    void receive();
                    TcpConnection();
            };

            private:
                bool connected = false;
                int maxConnectionBacklog = 5;
                int port;
                std::vector<std::shared_ptr<TcpConnection>> connections;
                std::thread acceptThread;
                std::mutex mutex;

                void accept();

            protected:
                virtual void onStart() override;
                virtual void onStop() override;
                virtual void onConnected() override {};
                virtual int onCreateSocketFd() override;
                virtual bool onReceive(std::shared_ptr<Data> data) override { return false; };
                void removeConnection(const TcpConnection* connection);
            public:
                virtual ~TcpServer() {};
                TcpServer(int port) : port(port) {signal(SIGPIPE, SIG_IGN);};

                void setMaxConectionBacklog(int max) { maxConnectionBacklog = max; }
                virtual void send(std::shared_ptr<IData> data) override;
                virtual bool isConnected() override;
        };
    }
}

#endif