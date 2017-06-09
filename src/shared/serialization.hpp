#pragma once

#include <cstring>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "socks.hpp"

struct SerializationBuffer
{
    std::vector<unsigned char> buffer;

    template<typename T> 
    void write32(const T& val)
    {
        buffer.resize(buffer.size() + 4);
        unsigned char *buffer_back = buffer.data() + buffer.size() - 4;
        unsigned int i_val = htonl(*reinterpret_cast<const unsigned int*>(&val));
        std::memcpy(buffer_back, &i_val, 4);
    }

    void write_vec3(const glm::vec3& v);
    void write_quat(const glm::quat& q);
};

class DeserializationBuffer
{
    std::vector<unsigned char> _buffer;
    int _read_head = 0;

public:
    DeserializationBuffer(const unsigned char *data, int len);

    template<typename T> 
    void read32(T& val)
    {
        const unsigned char *buffer_back = _buffer.data() + _read_head;
        unsigned int val_as_int = ntohl(*reinterpret_cast<const unsigned int*>(_buffer.data() + _read_head));
        std::memcpy(&val, &val_as_int, 4);
        _read_head += 4;
    }

    void read_vec3(glm::vec3& v);
    void read_quat(glm::quat& q);
};
