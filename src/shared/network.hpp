#pragma once

#include <string>
#include <cstdint>

struct SocketAddress
{
    uint16_t port;
    uint32_t address;
};

class UDPSocket
{
    int m_socket;

    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;

public:
    UDPSocket(uint16_t port = 0);
    ~UDPSocket();

    void send(const SocketAddress& remote_address, const uint8_t *buffer, int buffer_len) const;
    bool receive(SocketAddress& out_remote_address, uint8_t *buffer, int buffer_len, int& message_len) const;

    static SocketAddress get_host_address(const std::string& remote_host, uint16_t remote_port);
};
