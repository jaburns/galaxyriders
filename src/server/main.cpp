#include <chrono>
#include <thread>
#include <iostream>
#include <cstring>
#include <cstdint>
#include "../shared/network.hpp"
#include "../shared/world.hpp"
#include "../shared/config.hpp"

int main(int argc, char **argv)
{
    const int BUFFER_LEN = 2048;
    const int PORT = 12345;

    UDPSocket socket(PORT);
    SocketAddress client_address;
    int counter = 0;
    unsigned char buffer[BUFFER_LEN];

    std::cout << "Waiting for client on port " << PORT << std::endl;

    unsigned long cycles = 0;
    int message_len = 0;
    while (! socket.receive(client_address, buffer, BUFFER_LEN, message_len)) cycles++;
    std::cout << "Spun for: " << cycles << " Received message: " << buffer << std::endl;
    std::cout << "Starting simulation!" << std::endl;

    World world;
    InputState input;

    for (;;) {
        auto frame_start = std::chrono::high_resolution_clock::now();

        world = world.step(input);
        auto buf = world.serialize();
        socket.send(client_address, buf.data(), buf.size());
        std::cout << "Bytes sent: " << buf.size() << std::endl;

        auto frame_len = std::chrono::high_resolution_clock::now() - frame_start;
        auto nanos = static_cast<int64_t>(1000000 * Config::MILLIS_PER_TICK);
        auto sleep_time = std::chrono::nanoseconds(nanos) - frame_len;
        std::this_thread::sleep_for(sleep_time);
    }
}