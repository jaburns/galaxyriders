#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdint>
#include "gfx.hpp"
#include "state.hpp"
#include "readinput.hpp"
#include "renderer/game.hpp"
#include "../shared/network.hpp"
#include "../shared/config.hpp"

// Only needed until main_net is updated to use ClientState
#include "../shared/world.hpp"

void main_net()
{
    GameRenderer renderer;
    UDPSocket socket;
    const auto send_address = UDPSocket::get_host_address("localhost", Config::DEFAULT_PORT);
    SocketAddress receive_address;
    uint8_t buffer[Config::MAX_PACKET_SIZE];

    std::cout << "Sending ack packet to server port " << Config::DEFAULT_PORT << std::endl;
    sprintf((char*)buffer, "This is packet");
    socket.send(send_address, buffer, strlen((char*)buffer));

    World last_world, new_world;

    auto receive_world = std::chrono::high_resolution_clock::now();
    auto last_receive_world = std::chrono::high_resolution_clock::now();
    auto millis_per_tick = 100.0f;

    while (!Gfx::g_should_close_window) {
        int32_t message_len = 0;
        if (socket.receive(receive_address, buffer, Config::MAX_PACKET_SIZE, message_len)) {
            last_world = new_world;
            new_world = World(buffer, message_len);

            last_receive_world = receive_world;
            receive_world = std::chrono::high_resolution_clock::now();
            millis_per_tick = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(receive_world - last_receive_world).count());
        }

        const auto this_frame = std::chrono::high_resolution_clock::now();
        const auto millis_since_update = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(this_frame - receive_world).count());

        const auto inp = Input::read_state();
        if (inp.mouse_click) {
        //  renderer.render(last_world.lerp_to(new_world, millis_since_update / millis_per_tick));
        } else {
        //  renderer.render(new_world);
        }

        Gfx::flip_and_poll_events();
    }
}

void main_local()
{
    GameRenderer renderer;
    ClientState last_state, new_state;

    auto current_time = std::chrono::high_resolution_clock::now();
    auto accumulator = 0.0f;

    while (!Gfx::g_should_close_window) {
        const auto new_time = std::chrono::high_resolution_clock::now();
        const auto frame_millis = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(new_time - current_time).count());
        current_time = new_time;
        accumulator += frame_millis;

        while (accumulator >= Config::MILLIS_PER_TICK) {
            last_state = new_state;
            new_state = new_state.step(Input::read_state());
            accumulator -= Config::MILLIS_PER_TICK;
        }

        renderer.render(last_state.lerp_to(new_state, accumulator / Config::MILLIS_PER_TICK));

        Gfx::flip_and_poll_events();
    }
}

int main(int argc, char** argv)
{
    Gfx::init();
    Input::bind_handlers(nullptr);

    if (argc >= 2 && std::strcmp(argv[1], "net") == 0) {
        main_net();
    } else {
        main_local();
    }

    Gfx::deinit();

    return 0;
}