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

void test_network_client()
{
    const int BUFFER_LEN = 2048;
    const int PORT = 12345;

    SocketConnection socket;
    SocketAddress send_address = SocketConnection::get_host_address("localhost", PORT);
    SocketAddress receive_address;
    unsigned char buffer[BUFFER_LEN];

    for (int i=0; i < 5; i++) {
        std::cout << "Sending packet " << i << " to server port " << PORT << std::endl;
        sprintf((char*)buffer, "This is packet %d", i);
        socket.send(send_address, buffer, strlen((char*)buffer));

        unsigned long cycles = 0;
        while (! socket.receive(receive_address, buffer, BUFFER_LEN)) cycles++;
        std::cout << "Spun for: " << cycles << "   Received message: " << buffer << std::endl;
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
    test_network_client();
    wait_for_char();

    return 0;
}