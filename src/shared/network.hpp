#pragma once

#include <string>

struct SocketAddress
{
    unsigned short port;
    unsigned long address;
};

class UDPSocket
{
    int _socket;

    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;

public:
    UDPSocket(unsigned short port = 0);
    ~UDPSocket();

    void send(const SocketAddress& remote_address, const unsigned char *buffer, int buffer_len) const;
    bool receive(SocketAddress& out_remote_address, unsigned char *buffer, int buffer_len, int& message_len) const;

    static SocketAddress get_host_address(const std::string& remote_host, unsigned short remote_port);
};