#include "serialization.hpp"

void SerializationBuffer::vec3(const glm::vec3& v) 
{
    val32(v.x);
    val32(v.y);
    val32(v.z);
}

void SerializationBuffer::quat(const glm::quat& q) 
{
    val32(q.w);
    val32(q.x);
    val32(q.y);
    val32(q.z);
}

DeserializationBuffer::DeserializationBuffer(const unsigned char *data, int len)
{
    _buffer.resize(len);
    std::memcpy(_buffer.data(), data, len);
}

void DeserializationBuffer::vec3(glm::vec3& v) 
{
    val32(v.x);
    val32(v.y);
    val32(v.z);
}

void DeserializationBuffer::quat(glm::quat& q) 
{
    val32(q.w);
    val32(q.x);
    val32(q.y);
    val32(q.z);
}