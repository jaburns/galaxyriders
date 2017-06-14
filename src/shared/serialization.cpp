#include "serialization.hpp"

template<typename T> 
static void write_val32(SDBuffer::buffer_t& buffer, T& val)
{
    buffer.resize(buffer.size() + 4);
    unsigned char *buffer_back = buffer.data() + buffer.size() - 4;
    unsigned int i_val = htonl(*reinterpret_cast<const unsigned int*>(&val));
    std::memcpy(buffer_back, &i_val, 4);
}

template<typename T> 
static void read_val32(int& read_head, SDBuffer::buffer_t& buffer, T& val)
{
    const unsigned char *buffer_back = buffer.data() + read_head;
    unsigned int val_as_int = ntohl(*reinterpret_cast<const unsigned int*>(buffer.data() + read_head));
    std::memcpy(&val, &val_as_int, 4);
    read_head += 4;
}


void SerializationBuffer::float_val(float v) { write_val32(buffer, v); }
void SerializationBuffer::int32_val(int32_t v) { write_val32(buffer, v); }
int SerializationBuffer::deque_size(int32_t size_now, void (*resize)(int32_t size)) {
    write_val32(buffer, size_now);
    return size_now;
}

void DeserializationBuffer::float_val(float v) { read_val32(_read_head, buffer, v); }
void DeserializationBuffer::int32_val(int32_t v) { read_val32(_read_head, buffer, v); }
int DeserializationBuffer::deque_size(int32_t size_now, void (*resize)(int32_t size)) {
    int32_t size;
    read_val32(_read_head, buffer, size);
    resize(size);
    return size;
}

void SDBuffer::vec3_val(glm::vec3& v)
{
    float_val(v.x);
    float_val(v.y);
    float_val(v.z);
}

void SDBuffer::quat_val(glm::quat& v)
{
    float_val(v.w);
    float_val(v.x);
    float_val(v.y);
    float_val(v.z);
}