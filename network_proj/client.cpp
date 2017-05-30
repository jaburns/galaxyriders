#include <iostream>
#include <cstring>

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int LOCAL_PORT = 12345;
const int REMOTE_PORT = 12345;

#define BUFLEN 2048
#define MSGS 5	/* number of messages to send */

int main(int argc, char **argv)
{
    int fd;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "cannot create socket" << std::endl;
        return 1;
    }


    sockaddr_in addr;
    std::memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(LOCAL_PORT);
    if (bind(fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "bind failed" << std::endl;
        return 1;
    }


    sockaddr_in remaddr;
    std::memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(REMOTE_PORT);

    hostent *hp = gethostbyname("jaburns.net");
    if (!hp) {
        std::cout << "cannot get host address" << std::endl;
        return 1;
    }
    memcpy((void *)&remaddr.sin_addr, hp->h_addr_list[0], hp->h_length);


    char buf[BUFLEN];	/* message buffer */
	unsigned int slen=sizeof(remaddr);

    for (int i=0; i < MSGS; i++) {
        printf("Sending packet %d to server port %d\n", i, REMOTE_PORT);
        sprintf(buf, "This is packet %d", i);
        if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
            perror("sendto");
            return 1;
        }
        /* now receive an acknowledgement from the server */
        int recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
        if (recvlen >= 0) {
            buf[recvlen] = 0;	/* expect a printable string - terminate it */
            printf("received message: \"%s\"\n", buf);
        }
    }

    close(fd);

    return 0;
}