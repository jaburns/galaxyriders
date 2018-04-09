#include "network.hpp"

#include <iostream>
#include <cstring>
#include <cstdio>
#include "sockets.hpp"

bool SocketAddress::operator==(const SocketAddress& rhs) const
{
    return address == rhs.address && port == rhs.port;
}

bool SocketAddress::operator!=(const SocketAddress& rhs) const
{
    return !(*this == rhs);
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

SocketAddress SocketAddress::get_host_address(const std::string& remote_host, uint16_t remote_port)
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

static int build_socket(uint16_t port, int type)
{
    int result;

    #ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
            std::cout << "Failed to init winsock, error code: " << WSAGetLastError() << std::endl;
            exit(1);
        }
    #endif

    if ((result = socket(AF_INET, type, 0)) < 0) {
        std::cout << "Failed to create socket." << std::endl;
        exit(1);
    }

    #ifdef _WIN32
        u_long sock_mode_nonblocking = 1;
        ioctlsocket(result, FIONBIO, &sock_mode_nonblocking);
    #else
        fcntl(result, F_SETFL, O_NONBLOCK);
    #endif

    sockaddr_in myaddr;
    std::memset((char*)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port);

    if (bind(result, (sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
        std::cout << "Failed to bind socket to port " << port << std::endl;
        exit(1);
    }

    return result;
}

static void close_socket(int socket)
{
    #ifdef _WIN32
        closesocket(socket);
        WSACleanup();
    #else
        close(socket);
    #endif
}

UDPSocket::UDPSocket(uint16_t port)
    : m_socket(build_socket(port, SOCK_DGRAM))
{ }

UDPSocket::~UDPSocket()
{
    close_socket(m_socket);
}

void UDPSocket::send(const SocketAddress& remote_address, const uint8_t *buffer, int buffer_len) const
{
    sockaddr_in remaddr = to_sockaddr(remote_address);
    socklen_t slen = sizeof(remaddr);

    if (sendto(m_socket, (const char*)buffer, buffer_len, 0, (sockaddr *)&remaddr, slen) < 0) {
        std::cout << "Error sending message" << std::endl;
        exit(1);
    }
}

bool UDPSocket::receive(SocketAddress& out_remote_address, uint8_t *buffer, int buffer_len, int& message_len) const
{
    sockaddr_in remaddr;
    socklen_t slen = sizeof(remaddr);

    message_len = recvfrom(m_socket, (char*)buffer, buffer_len, 0, (sockaddr*)&remaddr, &slen);
    if (message_len < 0) return false;

    out_remote_address.address = remaddr.sin_addr.s_addr;
    out_remote_address.port = remaddr.sin_port;

    return true;
}

TCPServer::TCPServer(uint16_t port)
    : m_socket(build_socket(port, SOCK_STREAM))
{
    ::listen(m_socket, 0);
}

TCPServer::~TCPServer()
{
    close_socket(m_socket);
}

void TCPServer::listen(std::string(*callback)(const std::string&)) const
{
    sockaddr_in remaddr;
    socklen_t slen = sizeof(remaddr);
    char buffer[1024];

    int client = accept(m_socket, (sockaddr*)&remaddr, &slen);
    if (client < 0) return;

    memset(buffer, 0, sizeof(buffer));
    recv(client, buffer, sizeof(buffer) - 1, 0);

    std::string response = callback(buffer);

    send(client, response.c_str(), response.length(), 0);

    #ifdef _WIN32
        closesocket(client);
    #else
        close(client);
    #endif
}
