#pragma once

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
};
