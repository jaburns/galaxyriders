#pragma once

#include <cstring>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "sockets.hpp"

struct SerializationBuffer
{
    std::vector<unsigned char> buffer;

    template<typename T> 
    void val32(const T& val)
    {
        buffer.resize(buffer.size() + 4);
        unsigned char *buffer_back = buffer.data() + buffer.size() - 4;
        unsigned int i_val = htonl(*reinterpret_cast<const unsigned int*>(&val));
        std::memcpy(buffer_back, &i_val, 4);
    }

    template<typename T>
    int container_size(T& v)
    {
        int size = v.size();
        val32(size);
        return size;
    }
};

class DeserializationBuffer
{
    std::vector<unsigned char> _buffer;
    int _read_head = 0;

public:
    DeserializationBuffer(const unsigned char *data, int len) 
    {
        _buffer.resize(len);
        std::memcpy(_buffer.data(), data, len);
    }

    template<typename T> 
    void val32(T& val)
    {
        const unsigned char *buffer_back = _buffer.data() + _read_head;
        unsigned int val_as_int = ntohl(*reinterpret_cast<const unsigned int*>(_buffer.data() + _read_head));
        std::memcpy(&val, &val_as_int, 4);
        _read_head += 4;
    }

    template<typename T>
    int container_size(T& v)
    {
        int size;
        val32(size);
        v.resize(size);
        return size;
    }
};

template<typename T>
static void serialize_vec3(T& buffer, glm::vec3& v)
{
    buffer.val32(v.x);
    buffer.val32(v.y);
    buffer.val32(v.z);
}

template<typename T>
static void serialize_quat(T& buffer, glm::quat& q)
{
    buffer.val32(q.w);
    buffer.val32(q.x);
    buffer.val32(q.y);
    buffer.val32(q.z);
}
