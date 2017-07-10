#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include "fixed32.hpp"
#include "sockets.hpp"

struct SerializationBuffer
{
    std::vector<unsigned char> buffer;

    template<typename T> 
    void write_val32(const T& val)
    {
        buffer.resize(buffer.size() + 4);
        unsigned char *buffer_back = buffer.data() + buffer.size() - 4;
        uint32_t i_val = htonl(*reinterpret_cast<const uint32_t*>(&val));
        std::memcpy(buffer_back, &i_val, 4);
    }
};

class DeserializationBuffer
{
    std::vector<unsigned char> m_buffer;
    int m_read_head = 0;

public:
    DeserializationBuffer(const unsigned char *data, int len) 
    {
        m_buffer.resize(len);
        std::memcpy(m_buffer.data(), data, len);
    }

    template<typename T> 
    T read_val32()
    {
        const unsigned char *buffer_back = m_buffer.data() + m_read_head;
        uint32_t val_as_int = ntohl(*reinterpret_cast<const uint32_t*>(m_buffer.data() + m_read_head));
        T val;
        std::memcpy(&val, &val_as_int, 4);
        m_read_head += 4;
        return val;
    }
};