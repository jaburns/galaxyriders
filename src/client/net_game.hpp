#pragma once

#include <string>
#include "../shared/world.hpp"
#include "../shared/network.hpp"
#include "../shared/lang_utils.hpp"
#include "../shared/config.hpp"

class NetGame : public NoCopy
{
    UDPSocket socket;
    SocketAddress receive_address;
    SocketAddress send_address;

    uint8_t buffer[Config::MAX_PACKET_SIZE];

public:
    void connect(const std::string& remote_host, uint16_t port);
    bool update(const PlayerInput& input, World& received_world);
};
