#include "net_game.hpp"

#include "../shared/serialization.hpp"
#include <iostream>
#include <chrono>
#include <thread>

static const uint8_t JOIN_REQUEST = 0xFF;
static const uint8_t SEND_INPUT = 0x01;

int32_t NetGame::connect(const std::string& remote_host, uint16_t port)
{
    m_send_address = SocketAddress::get_host_address(remote_host, port);

    std::cout << "Requesting to join server on port " << Config::DEFAULT_PORT << std::endl;
    m_buffer[0] = JOIN_REQUEST;
    m_buffer[1] = 0;
    m_socket.send(m_send_address, m_buffer, 1);

    int32_t message_len = 0;
    while (!m_socket.receive(m_receive_address, m_buffer, Config::MAX_PACKET_SIZE, message_len)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    DeserializationBuffer db(m_buffer, message_len);
    const auto player_id = db.read_val32<int32_t>();

    std::cout << "Received player ID: " << player_id << std::endl;
    std::cout << "Starting game!" << std::endl;

    return player_id;
}

bool NetGame::update(const PlayerInput& input, WorldState& received_world)
{
    int32_t message_len = 0;

    if (!m_socket.receive(m_receive_address, m_buffer, Config::MAX_PACKET_SIZE, message_len)) {
        return false;
    }

    received_world = WorldState(m_buffer, message_len);

    const auto input_buf = input.serialize();
    std::memset(&m_buffer, 0, sizeof(m_buffer));
    m_buffer[0] = SEND_INPUT;
    memcpy(&m_buffer[1], input_buf.data(), input_buf.size());
    m_socket.send(m_send_address, m_buffer, input_buf.size() + 1);

    return true;
}
