#include <iostream>
#include <chrono>
#include "readinput.hpp"
#include "render.hpp"
#include "../shared/network.hpp"

static const int MAX_BUFFER_LEN = 8192;
static const int PORT = 12345;

int main(int argc, char** argv)
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

    #ifdef _WIN32
        std::cout<<"Enter a char to quit";char _;std::cin>>_;
    #endif

    return 0;
}