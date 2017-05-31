#include <iostream>
#include <cstring>
#include <cstdio>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#   define WINDOWS 1
#endif 

#ifdef WINDOWS
#   include <winsock2.h>
    typedef int socklen_t;
#else
#   include <unistd.h>
#   include <netdb.h>
#   include <sys/socket.h>
#   include <arpa/inet.h>
#endif

const int BUFFER_LEN = 2048;
const int LOCAL_PORT = 12345;

int server_main(int argc, char **argv)
{
#ifdef WINDOWS
    WSADATA wsa;
    std::cout << "Initialising Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        std::cout << "Failed. Error code: " << WSAGetLastError() << std::endl;
        return 1;
    }
    std::cout << "Initialised." << std::endl;
#endif

    int fd;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Cannot create socket" << std::endl;
        return 1;
    }

    sockaddr_in myaddr;
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(LOCAL_PORT);
    if (bind(fd, (sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        std::cout << "Bind failed" << std::endl;
        return 1;
    }

    int msgcnt = 0;
    sockaddr_in remaddr;
    char buf[BUFFER_LEN];
    socklen_t slen = sizeof(remaddr);

    for (;;) {
        std::cout << "Waiting on port " << LOCAL_PORT << std::endl;
        int recvlen = recvfrom(fd, buf, BUFFER_LEN, 0, (sockaddr *)&remaddr, &slen);
        if (recvlen > 0) {
            buf[recvlen] = 0;
            std::cout << "received message: " << buf << " :: byte count: " << recvlen << std::endl;
        } else {
            std::cout << "Error receiving message" << std::endl;
            return 1;
        }
        sprintf(buf, "Ack %d", msgcnt++);
        std::cout << "Sending response " << buf << std::endl;
        if (sendto(fd, buf, strlen(buf), 0, (sockaddr *)&remaddr, slen) < 0) {
            std::cout << "Error sending message" << std::endl;
            return 1;
        }
    }

    // Never gets here.

#ifdef WINDOWS
    closesocket(fd);
    WSACleanup();
#else
    close(fd);
#endif
}
