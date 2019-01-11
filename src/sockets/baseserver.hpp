#ifndef DT_SOCKET_BASE_H
#define DT_SOCKET_BASE_H

// Loosely based off of:
// https://github.com/Loki-Astari/Examples/blob/master/Version2

#include <sys/socket.h>
#include <vector>
#include <algorithm>

#include "datawriter.hpp"
#include "datareader.hpp"

namespace DoubTech {
    namespace Sockets {
        class BaseServer : public DataWriter {
            private:
                int socketFd;
                std::vector<std::weak_ptr<DataReader>> dataReaders;

                void cleanExpired();
            protected:
                virtual void onStop() = 0;
                virtual void onStart() = 0;
                virtual void onConnected() = 0;
                virtual int onCreateSocketFd() = 0;
                /**
                 * Virtual called from processReceivedData
                 * 
                 * Note: Do not call this directly
                 * 
                 * Return true if data has been processed andn listeners
                 * should not be called.
                 */
                virtual bool onReceive(std::shared_ptr<Data> data) = 0;

                /**
                 * Send processed data out to child classes and subscribed data
                 * readers
                 **/
                virtual void processReceivedData(std::shared_ptr<Data> data);

                int getSocketFd() { return socketFd; };
                void invalidateSocketFd() { socketFd = INVALID_SOCKET_ID; };

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
                virtual bool isConnected() = 0;

                virtual void send(std::shared_ptr<IData> data) = 0;

                void addDataReader(std::weak_ptr<DataReader> reader) {
                    dataReaders.push_back(reader);
                }

                void removeDataReader(std::weak_ptr<DataReader> reader);
        };
    }
}

#endif