#include <iostream>
#include "src/network.hpp"

int main(int argc, char **argv)
{
    SocketConnection s(12345);
    s.server_loop();
}