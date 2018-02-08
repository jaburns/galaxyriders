#include <chrono>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <cstring>
#include <cstdint>
#include "../shared/network.hpp"
#include "../shared/serialization.hpp"
#include "../shared/world.hpp"
#include "../shared/config.hpp"
#include "../shared/debug.hpp"

static std::unordered_map<int32_t, SocketAddress> connections;

int main(int argc, char **argv)
{
    UDPSocket socket(Config::DEFAULT_PORT);
    SocketAddress client_address;
    int counter = 0;
    uint8_t buffer[Config::MAX_PACKET_SIZE];

    std::cout << "GalaxyRiders Server - Built " << __TIME__ << std::endl;
    std::cout << "Waiting for clients on port " << Config::DEFAULT_PORT << std::endl;

    int32_t message_len = 0;
    while (!socket.receive(client_address, buffer, Config::MAX_PACKET_SIZE, message_len)) {}

    if (buffer[0] != 0xFF) {
        std::cout << "UNEXPECTED PACKET";
        exit(1);
    }

    std::cout << "Received request to join game from " << client_address.address << ":" << client_address.port << std::endl;
    std::cout << "Sending player ID: " << 1337 << std::endl;

    SerializationBuffer sb;
    sb.write_val32<int32_t>(1337);

    socket.send(client_address, sb.buffer.data(), sb.buffer.size());

    std::cout << "Starting simulation!" << std::endl;

    World world;
    PlayerInput last_input, current_input;

    world.players[1337];

    for (;;) {
        const auto frame_start = std::chrono::high_resolution_clock::now();

        while (socket.receive(client_address, buffer, Config::MAX_PACKET_SIZE, message_len)) {
            if (buffer[0] != 0x01) {
                std::cout << "UNEXPECTED PACKET";
                exit(1);
            }

            last_input = current_input;
            current_input = PlayerInput(&buffer[1], message_len);
        }

        world.step(last_input, current_input);

        Debug::log(world.players.size(), world.players[1337].position.y);

        const auto buf = world.serialize();
        socket.send(client_address, buf.data(), buf.size());

        const auto frame_len = std::chrono::high_resolution_clock::now() - frame_start;
        const auto nanos = static_cast<int64_t>(1000000 * Config::MILLIS_PER_TICK);
        const auto sleep_time = std::chrono::nanoseconds(nanos) - frame_len;
        std::this_thread::sleep_for(sleep_time);
    }
}