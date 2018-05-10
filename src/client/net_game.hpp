#pragma once

#include <string>
#include "../shared/world_state.hpp"
#include "../shared/network.hpp"
#include "../shared/lang_utils.hpp"
#include "../shared/config.hpp"

class NetGame : public NoCopy
{
    UDPSocket m_socket;
    SocketAddress m_receive_address;
    SocketAddress m_send_address;

    uint8_t m_buffer[Config::MAX_PACKET_SIZE];

public:
    int32_t connect(const std::string& remote_host, uint16_t port);
    bool update(const PlayerInput& input, WorldState& received_world);
};
