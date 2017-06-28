#include <iostream>
#include <chrono>
#include <cstring>
#include "readinput.hpp"
#include "render.hpp"
#include "../shared/network.hpp"

static const int MAX_BUFFER_LEN = 8192;
static const int PORT = 12345;

void main_net()
{
    Renderer renderer;
    Input::bind_handlers(renderer.raw_glfw_window());

    UDPSocket socket;
    SocketAddress send_address = UDPSocket::get_host_address("localhost", PORT);
    SocketAddress receive_address;
    unsigned char buffer[MAX_BUFFER_LEN];

    std::cout << "Sending ack packet to server port " << PORT << std::endl;
    sprintf((char*)buffer, "This is packet");
    socket.send(send_address, buffer, strlen((char*)buffer));

    World last_world, new_world;

    auto receive_world = std::chrono::high_resolution_clock::now();
    auto last_receive_world = std::chrono::high_resolution_clock::now();
    float millis_per_tick = 100.0f;

    while (!renderer.should_close_window()) {
        int message_len = 0;
        if (socket.receive(receive_address, buffer, MAX_BUFFER_LEN, message_len)) {
            last_world = new_world;
            new_world = World(buffer, message_len);

            last_receive_world = receive_world;
            receive_world = std::chrono::high_resolution_clock::now();
            millis_per_tick = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(receive_world - last_receive_world).count());
        }

        auto this_frame = std::chrono::high_resolution_clock::now();
        float millis_since_update = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(this_frame - receive_world).count());

        auto inp = Input::read_state();
        if (inp.clicking) {
            renderer.render(last_world.lerp_to(new_world, millis_since_update / millis_per_tick));
        } else {
            renderer.render(new_world);
        }
    }
}

void main_local()
{
    Renderer renderer;
    Input::bind_handlers(renderer.raw_glfw_window());

    World last_world, new_world;

    auto current_time = std::chrono::high_resolution_clock::now();
    float accumulator = 0.0f;
    const float MILLIS_PER_TICK = 100.0f;

    while (!renderer.should_close_window()) {
        const auto new_time = std::chrono::high_resolution_clock::now();
        const float frame_millis = static_cast<const float>(std::chrono::duration_cast<std::chrono::milliseconds>(new_time - current_time).count());
        current_time = new_time;
        accumulator += frame_millis;

        while (accumulator >= MILLIS_PER_TICK) {
            last_world = new_world;
            new_world = new_world.step(Input::read_state());
            accumulator -= MILLIS_PER_TICK;
        }

        renderer.render(last_world.lerp_to(new_world, accumulator / MILLIS_PER_TICK));
    }
}

int main(int argc, char** argv)
{
    if (argc >= 2 && std::strcmp(argv[1], "net") == 0) {
        main_net();
    } else {
        main_local();
    }

    #ifdef _WIN32
        std::cout<<"Enter a char to quit";char _;std::cin>>_;
    #endif

    return 0;
}