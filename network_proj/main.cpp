#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 1337;

int main(int argc, char **argv)
{
    int fd;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "cannot create socket" << std::endl;
        return 1;
    }

    sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "bind failed" << std::endl;
        return 1;
    }

    std::cout << "created socket: descriptor=" << fd << std::endl;

    close(fd);

    return 0;
}
