#pragma once

struct SocketAddress
{
    unsigned short port;
    unsigned long address;
};

class SocketConnection
{
    unsigned short _port;
    int _socket;

    SocketConnection(const SocketConnection&);
    SocketConnection& operator=(const SocketConnection&);

public:
    SocketConnection(unsigned short port = 0);
    ~SocketConnection();

    void client_loop(const char *remote_host, unsigned short remote_port);
    void server_loop();

    void send(const SocketAddress& remote_address, const char *buffer, int buffer_len);
    bool receive(SocketAddress& out_remote_address, char *buffer, int buffer_len);

    static SocketAddress get_host_address(const char *remote_host, unsigned short remote_port);
};
