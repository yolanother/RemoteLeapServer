#include "udpserver.hpp"
#include "../utilities/StringUtils.hpp"
#include "../utilities/logging.hpp"

#include "datawriter.hpp"

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <thread>

using namespace DoubTech::Sockets;
using namespace ThorsAnvil::Socket;

#define TAG "UdpServer"
/*
void UdpServer::onStart() {
    
}

void UdpServer::onStop() {
    
}

bool UdpServer::onReceive(const char* data, size_t len) {
    return false;
}

int UdpServer::onCreateSocketFd() {
    logd("Creating socket...");
    int socketFd = ::socket(PF_INET, SOCK_DGRAM, 0);

    if(socketFd == INVALID_SOCKET_ID) {
        throwRuntime(": Error creating socket handle.", strerror(errno));
    }
    logd("  - done");


    struct sockaddr_in serverAddr;
    bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = INADDR_ANY;

    logd("Binding to port ", port);
    if (::bind(socketFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0) {
        ::close(socketFd);
        throwRuntime("bind: ", strerror(errno));
    }
    logd("  - done.");

    return socketFd;
}

void UdpServer::send(Data data) {
    struct sockaddr_in cliaddr; 

    int socketFd = getSocketFd();
    logd("About to send...");
    if(INVALID_SOCKET_ID != socketFd) {
        logd("Sending ", (int) data.length(), " bytes to ", (int) socketFd);
        ::send(socketFd, data.data(), data.length(), 0); 
    }
}
*/