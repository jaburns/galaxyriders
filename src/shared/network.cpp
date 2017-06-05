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
#   include <fcntl.h>
#endif

SocketConnection::SocketConnection(unsigned short port)
{
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

    #ifdef WINDOWS
        unsigned long sock_mode_nonblocking = 1;
        ioctlsocket(_socket, FIONBIO, &sock_mode_nonblocking);
    #else 
        fcntl(_socket, F_SETFL, O_NONBLOCK);
    #endif 

    sockaddr_in myaddr;
    std::memset((char*)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port);

    if (bind(_socket, (sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
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

static sockaddr_in to_sockaddr(const SocketAddress& socket_address)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = socket_address.address;
    addr.sin_port = socket_address.port;
    return addr;
}

static SocketAddress from_sockaddr(const sockaddr_in& addr)
{
    SocketAddress result;
    result.address = addr.sin_addr.s_addr;
    result.port = addr.sin_port;
    return result;
}

SocketAddress SocketConnection::get_host_address(const std::string& remote_host, unsigned short remote_port)
{
    sockaddr_in remaddr;
    std::memset((char*)&remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(remote_port);

    hostent *hp = gethostbyname(remote_host.c_str());
    if (!hp) {
        std::cout << "Cannot get host address: " << remote_host << ":" << remote_port << std::endl;
        exit(1);
    }
    std::memcpy((void*)&remaddr.sin_addr, hp->h_addr_list[0], hp->h_length);
    
    return from_sockaddr(remaddr);
}

void SocketConnection::send(const SocketAddress& remote_address, const unsigned char *buffer, int buffer_len) const
{
    sockaddr_in remaddr = to_sockaddr(remote_address);
    socklen_t slen = sizeof(remaddr);

    if (sendto(_socket, (const char*)buffer, buffer_len, 0, (sockaddr *)&remaddr, slen) < 0) {
        std::cout << "Error sending message" << std::endl;
        exit(1);
    }
}

bool SocketConnection::receive(SocketAddress& out_remote_address, unsigned char *buffer, int buffer_len) const
{
    sockaddr_in remaddr;
    socklen_t slen = sizeof(remaddr);

    int recvlen = recvfrom(_socket, (char*)buffer, buffer_len, 0, (sockaddr*)&remaddr, &slen);
    if (recvlen < 0) return false;

    buffer[recvlen] = 0;

    out_remote_address.address = remaddr.sin_addr.s_addr;
    out_remote_address.port = remaddr.sin_port;

    return true;
}