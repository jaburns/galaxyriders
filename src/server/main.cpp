#include <iostream>
#include <cstring>
#include "../shared/network.hpp"

int main(int argc, char **argv)
{
    const int BUFFER_LEN = 2048;
    const int PORT = 12345;

    SocketConnection socket(PORT);
    SocketAddress client_address;
    int counter = 0;
    unsigned char buffer[BUFFER_LEN];

    for (;;) {
        std::cout << "Listening on port " << PORT << std::endl;

        unsigned long cycles = 0;
        while (! socket.receive(client_address, buffer, BUFFER_LEN)) cycles++;
        std::cout << "Spun for: " << cycles << "   Received message: " << buffer << std::endl;

        sprintf((char*)buffer, "Ack %d", counter++);
        std::cout << "Sending response: " << buffer << std::endl;
        socket.send(client_address, buffer, strlen((char*)buffer));
    }
}
