#pragma once

#include <string>

struct SocketAddress
{
    unsigned short port;
    unsigned long address;
};

class SocketConnection
{
    int _socket;

    SocketConnection(const SocketConnection&);
    SocketConnection& operator=(const SocketConnection&);

public:
    SocketConnection(unsigned short port = 0);
    ~SocketConnection();

    void send(const SocketAddress& remote_address, const unsigned char *buffer, int buffer_len) const;
    bool receive(SocketAddress& out_remote_address, unsigned char *buffer, int buffer_len, int& message_len) const;

    static SocketAddress get_host_address(const std::string& remote_host, unsigned short remote_port);
};
