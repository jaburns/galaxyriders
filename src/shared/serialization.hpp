#pragma once

#include <cstring>

// TODO move socket includes and function defs in to cpp file

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
#   include <fcntl.h>
#endif

static inline void write_float(unsigned char *buffer, int index, float f) 
{
    unsigned int f_as_int;
    std::memcpy(&f_as_int, &f, sizeof(f_as_int));
    unsigned int f_net = htonl(f_as_int);
    std::memcpy(&buffer[index], &f_net, sizeof(f_net));
}

static inline float read_float(unsigned char *buffer, int index) 
{
    unsigned int f_net;
    std::memcpy(&f_net, &buffer[index], sizeof(f_as_int));
    unsigned int f_as_int = ntohl(f_net);
    float result;
    std::memcpy(&result, &f_as_int, sizeof(result));
    return result;
}