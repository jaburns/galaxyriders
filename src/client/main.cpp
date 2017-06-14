#include <iostream>
#include <chrono>
#include <thread>
#include "readinput.hpp"
#include "render.hpp"
#include "../shared/network.hpp"

void render_from_server()
{
    Renderer renderer;
    World last_world, new_world;
    float world_lerp = 0.0f;
    Input::bind_handlers(renderer.raw_glfw_window());

    const int MAX_BUFFER_LEN = 8192;
    const int PORT = 12345;

    UDPSocket socket;
    SocketAddress send_address = UDPSocket::get_host_address("localhost", PORT);
    SocketAddress receive_address;
    unsigned char buffer[MAX_BUFFER_LEN];

    std::cout << "Sending ack packet to server port " << PORT << std::endl;
    sprintf((char*)buffer, "This is packet");
    socket.send(send_address, buffer, strlen((char*)buffer));

    auto receive_world = std::chrono::high_resolution_clock::now();

    while (!renderer.should_close_window()) {
        int message_len = 0;
        if (socket.receive(receive_address, buffer, MAX_BUFFER_LEN, message_len)) {
            last_world = new_world;
            new_world = World(buffer, message_len);
            world_lerp = 0.0f;

            receive_world = std::chrono::high_resolution_clock::now();
        }

        auto this_frame = std::chrono::high_resolution_clock::now();
        auto millis_since_update = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(this_frame - receive_world).count());
        float world_lerp = millis_since_update / 100.0f; // (100 = ms per tick) TODO measure TPS from incoming packet rate

           renderer.render(last_world.lerp_to(new_world, world_lerp));
        //renderer.render(new_world);
    }
}

void wait_for_char()
{
    using namespace std;
    cout << "Enter a char to quit";
    char x;
    cin >> x;
}

int main(int argc, char** argv)
{
    render_from_server();
//  wait_for_char();
    return 0;
}
