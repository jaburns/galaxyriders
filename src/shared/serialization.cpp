#include "serialization.hpp"

#include <cstring>
#include "socks.hpp"

template<typename T> 
void SerializationBuffer::write32(const T& val)
{
    buffer.resize(buffer.size() + 4);
    unsigned char *buffer_back = buffer.data() + buffer.size() - 4;
    unsigned int i_val = htonl(*reinterpret_cast<const unsigned int*>(&val));
    std::memcpy(buffer_back, &i_val, 4);
}

void SerializationBuffer::write_vec3(const glm::vec3& v) 
{
    write32(v.x);
    write32(v.y);
    write32(v.z);
}

void SerializationBuffer::write_quat(const glm::quat& q) 
{
    write32(q.w);
    write32(q.x);
    write32(q.y);
    write32(q.z);
}

DeserializationBuffer::DeserializationBuffer(const unsigned char *data, int len)
{
    _buffer.resize(len);
    std::memcpy(_buffer.data(), data, len);
}

template<typename T> 
void DeserializationBuffer::read32(T& val)
{
    const unsigned char *buffer_back = _buffer.data() + _read_head;
    unsigned int val_as_int = ntohl(*reinterpret_cast<const unsigned int*>(_buffer.data() + _read_head));
    std::memcpy(&val, &val_as_int, 4);
    _read_head += 4;
}

void DeserializationBuffer::read_vec3(glm::vec3& v) 
{
    read32(v.x);
    read32(v.y);
    read32(v.z);
}

void DeserializationBuffer::read_quat(glm::quat& q) 
{
    read32(q.w);
    read32(q.x);
    read32(q.y);
    read32(q.z);
}