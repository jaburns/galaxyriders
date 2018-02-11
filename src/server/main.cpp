#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "../shared/network.hpp"
#include "../shared/serialization.hpp"
#include "../shared/world.hpp"
#include "../shared/config.hpp"
#include "../shared/debug.hpp"

struct ConnectedPlayer
{
    int32_t player_id;
    SocketAddress address;
    PlayerInput last_input;
    PlayerInput current_input;
};

static int32_t player_id_counter = 0;
static World world;
static uint8_t buffer[Config::MAX_PACKET_SIZE];
static std::vector<ConnectedPlayer> connections;

static void send_join_response(const UDPSocket& socket, const ConnectedPlayer& player)
{
    SerializationBuffer sb;
    sb.write_val32<int32_t>(player.player_id);
    socket.send(player.address, sb.buffer.data(), sb.buffer.size());
}

static void handle_incoming_packets(const UDPSocket& socket, World& world)
{
    SocketAddress client_address;
    int32_t message_len = 0;

    while (socket.receive(client_address, buffer, Config::MAX_PACKET_SIZE, message_len)) {
        switch (buffer[0]) {
            case 0xFF:
            {
                std::cout << "Received request to join game from " << client_address.address << ":" << client_address.port << std::endl;
                std::cout << "Sending player ID: " << player_id_counter << std::endl;

                world.players[player_id_counter];

                ConnectedPlayer new_player;
                new_player.address = client_address;
                new_player.player_id = player_id_counter;

                player_id_counter++;

                send_join_response(socket, new_player);
                connections.push_back(new_player);

                break;
            }

            case 0x01:
            {
                for (auto& p : connections) {
                    if (p.address != client_address) continue;

                    p.last_input = p.current_input;
                    p.current_input = PlayerInput(&buffer[1], message_len);

                    break;
                }
            }
        }
    }
}

static std::vector<World::Input> get_inputs()
{
    std::vector<World::Input> result;

    for (const auto& connection : connections) {
        World::Input input;
        input.player_id = connection.player_id;
        input.old_input = connection.last_input;
        input.new_input = connection.current_input;
        result.push_back(input);
    }

    return result;
}

int main(int argc, char **argv)
{
    UDPSocket socket(Config::DEFAULT_PORT);

    std::cout << "GalaxyRiders Server - Built " << __TIME__ << std::endl;
    std::cout << "Waiting for clients on port " << Config::DEFAULT_PORT << std::endl;
    std::cout << "Starting simulation!" << std::endl;

    for (;;) {
        const auto frame_start = std::chrono::high_resolution_clock::now();

        handle_incoming_packets(socket, world);
        world.step(get_inputs());

        const auto buf = world.serialize();
        for (const auto& connection : connections) {
            socket.send(connection.address, buf.data(), buf.size());
        }

        const auto frame_len = std::chrono::high_resolution_clock::now() - frame_start;
        const auto nanos = static_cast<int64_t>(1000000 * Config::MILLIS_PER_TICK);
        const auto sleep_time = std::chrono::nanoseconds(nanos) - frame_len;
        std::this_thread::sleep_for(sleep_time);
    }
}