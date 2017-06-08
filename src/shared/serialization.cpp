#include "serialization.hpp"

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