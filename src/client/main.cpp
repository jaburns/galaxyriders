#include "readinput.hpp"
#include "render.hpp"
#include "../shared/network.hpp"

#include <iostream>

using namespace std;

void run()
{
    Renderer renderer;
    World world;
    Input::bind_handlers(renderer.raw_glfw_window());

    while (!renderer.should_close_window()) {
        auto input_state = Input::read_state();
        world = world.step(input_state);
        renderer.render(world);
    }
}

void render_from_server()
{
    const int MAX_BUFFER_LEN = 8192;
    const int PORT = 12345;

    SocketConnection socket;
    SocketAddress send_address = SocketConnection::get_host_address("localhost", PORT);
    SocketAddress receive_address;
    unsigned char buffer[MAX_BUFFER_LEN];

    std::cout << "Sending ack packet to server port " << PORT << std::endl;
    sprintf((char*)buffer, "This is packet");
    socket.send(send_address, buffer, strlen((char*)buffer));

    Renderer renderer;
    World world;
    Input::bind_handlers(renderer.raw_glfw_window());

    while (!renderer.should_close_window()) {
        int message_len = 0;
        if (socket.receive(receive_address, buffer, MAX_BUFFER_LEN, message_len)) {
            std::cout << "Bytes received: " << message_len << std::endl;
            world = World(buffer, message_len);
        }
        renderer.render(world);
    }
}

void wait_for_char()
{
    cout << "Enter a char to quit";
    char x;
    cin >> x;
}

int main(int argc, char** argv)
{
//  run();
//  test_serialization();
    render_from_server();
//  wait_for_char();

    return 0;
}
