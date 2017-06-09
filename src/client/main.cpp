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
    Renderer renderer;
    World world;
    Input::bind_handlers(renderer.raw_glfw_window());

    const int BUFFER_LEN = 2048;
    const int PORT = 12345;

    SocketConnection socket;
    SocketAddress send_address = SocketConnection::get_host_address("localhost", PORT);
    SocketAddress receive_address;
    unsigned char buffer[BUFFER_LEN];

    std::cout << "Sending ack packet to server port " << PORT << std::endl;
    sprintf((char*)buffer, "This is packet");
    socket.send(send_address, buffer, strlen((char*)buffer));

    while (!renderer.should_close_window()) {
        int message_len = 0;
        if (socket.receive(receive_address, buffer, BUFFER_LEN, message_len)) {
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
    wait_for_char();

    return 0;
}