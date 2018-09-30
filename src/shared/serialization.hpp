#pragma once
#include "global.hpp"

#include <cstdint>
#include <cstring>
#include <vector>
#include "sockets.hpp"

struct SerializationBuffer
{
    std::vector<uint8_t> buffer;

    template<typename T>
    void write_val32(T val)
    {
        buffer.resize(buffer.size() + 4);
        uint8_t *buffer_back = buffer.data() + buffer.size() - 4;
        uint32_t i_val = htonl(*reinterpret_cast<const uint32_t*>(&val));
        std::memcpy(buffer_back, &i_val, 4);
    }

    void write_byte(uint8_t byte)
    {
        buffer.push_back(byte);
    }
};

class DeserializationBuffer
{
    std::vector<uint8_t> m_buffer;
    int m_read_head = 0;

public:
    DeserializationBuffer(const uint8_t *data, int len)
    {
        m_buffer.resize(len);
        std::memcpy(m_buffer.data(), data, len);
    }

    template<typename T>
    T read_val32()
    {
        const uint8_t *buffer_back = m_buffer.data() + m_read_head;
        uint32_t val_as_int = ntohl(*reinterpret_cast<const uint32_t*>(buffer_back));
        T val;
        std::memcpy(&val, &val_as_int, 4);
        m_read_head += 4;
        return val;
    }

    uint8_t read_byte()
    {
        const uint8_t *buffer_back = m_buffer.data() + m_read_head;
        uint8_t val = *buffer_back;
        m_read_head++;
        return val;
    }
};