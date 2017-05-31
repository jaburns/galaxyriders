#include "network.hpp"

#include <iostream>
#include <cstring>
#include <cstdio>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#   define WINDOWS 1
#endif 

#ifdef WINDOWS
#   include <winsock2.h>
    typedef int socklen_t;
#else
#   include <unistd.h>
#   include <netdb.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#endif

const int BUFFER_LEN = 2048;

SocketConnection::SocketConnection(unsigned short port)
{
    _port = port;

    #ifdef WINDOWS
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
            std::cout << "Failed to init winsock, error code: " << WSAGetLastError() << std::endl;
            exit(1);
        }
    #endif

    if ((_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Failed to create socket." << std::endl;
        exit(1);
    }

    sockaddr_in myaddr;
    std::memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port);

    if (bind(_socket, (sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        std::cout << "Failed to bind socket to port " << port << std::endl;
        exit(1);
    }
}

SocketConnection::~SocketConnection()
{
    #ifdef WINDOWS
        closesocket(_socket);
        WSACleanup();
    #else
        close(_socket);
    #endif
}

void SocketConnection::client_loop(const char *remote_host, unsigned short remote_port)
{
    sockaddr_in remaddr;
    std::memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(remote_port);

    hostent *hp = gethostbyname(remote_host);
    if (!hp) {
        std::cout << "Cannot get host address" << std::endl;
        exit(1);
    }
    std::memcpy((void *)&remaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

    char buf[BUFFER_LEN];
    socklen_t slen=sizeof(remaddr);

    for (int i=0; i < 5; i++) {
        std::cout << "Sending packet " << i << " to server port " << remote_port << std::endl;
        sprintf(buf, "This is packet %d", i);
        if (sendto(_socket, buf, strlen(buf), 0, (sockaddr *)&remaddr, slen) < 0) {
            std::cout << "Error sending message" << std::endl;
            exit(1);
        }
        int recvlen = recvfrom(_socket, buf, BUFFER_LEN, 0, (sockaddr *)&remaddr, &slen);
        if (recvlen >= 0) {
            buf[recvlen] = 0;
            std::cout << "Received message: " << buf << std::endl;
        } else {
            std::cout << "Error receiving message" << std::endl;
            exit(1);
        }
    }
}

void SocketConnection::server_loop()
{
    int msgcnt = 0;
    sockaddr_in remaddr;
    char buf[BUFFER_LEN];
    socklen_t slen = sizeof(remaddr);

    for (;;) {
        std::cout << "Listening on port " << _port << std::endl;
        int recvlen = recvfrom(_socket, buf, BUFFER_LEN, 0, (sockaddr *)&remaddr, &slen);
        if (recvlen > 0) {
            buf[recvlen] = 0;
            std::cout << "Received message: " << buf << " :: byte count: " << recvlen << std::endl;
        } else {
            std::cout << "Error receiving message" << std::endl;
            exit(1);
        }
        sprintf(buf, "Ack %d", msgcnt++);
        std::cout << "Sending response " << buf << std::endl;
        if (sendto(_socket, buf, strlen(buf), 0, (sockaddr *)&remaddr, slen) < 0) {
            std::cout << "Error sending message" << std::endl;
            exit(1);
        }
    }
}