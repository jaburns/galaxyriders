#include "serialization.hpp"

#include <cstring>
#include "socks.hpp"

void SerializationBuffer::write_float(float& f) 
{
    buffer.resize(buffer.size() + 4);
    unsigned char *buffer_back = buffer.data() + buffer.size() - 4;

    unsigned int f_as_int;
    std::memcpy(&f_as_int, &f, sizeof(f_as_int));
    unsigned int f_net = htonl(f_as_int);
    std::memcpy(buffer_back, &f_net, sizeof(f_net));
}

void SerializationBuffer::write_vec3(glm::vec3& v) 
{
    write_float(v.x);
    write_float(v.y);
    write_float(v.z);
}

DeserializationBuffer::DeserializationBuffer(const unsigned char *data, int len)
{
    _buffer.resize(len);
    std::memcpy(_buffer.data(), data, len);
}

float DeserializationBuffer::read_float() 
{
    const unsigned char *buffer_back = _buffer.data() + _read_head;

    unsigned int f_net;
    std::memcpy(&f_net, buffer_back, sizeof(f_net));
    unsigned int f_as_int = ntohl(f_net);
    float result;
    std::memcpy(&result, &f_as_int, sizeof(result));

    _read_head += 4;

    return result;
}

glm::vec3 DeserializationBuffer::read_vec3() 
{
    glm::vec3 ret;
    ret.x = read_float();
    ret.y = read_float();
    ret.z = read_float();
    return ret;
}