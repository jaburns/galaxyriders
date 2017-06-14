#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <deque>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "sockets.hpp"

struct SDBuffer
{
    typedef std::vector<unsigned char> buffer_t;

    buffer_t buffer;

    virtual void float_val(float v) =0;
    virtual void int32_val(int32_t v) =0;
    virtual int deque_size(int32_t size_now, void (*resize)(int32_t size)) =0;

    void vec3_val(glm::vec3& v);
    void quat_val(glm::quat& v);
};

struct SerializationBuffer : public SDBuffer
{ 
    void float_val(float v);
    void int32_val(int32_t v);
    int deque_size(int32_t size_now, void (*resize)(int32_t size));
};

struct DeserializationBuffer : public SDBuffer
{
    void float_val(float v);
    void int32_val(int32_t v);
    int deque_size(int32_t size_now, void (*resize)(int32_t size));

    DeserializationBuffer(const unsigned char *data, int len) {
        buffer.resize(len);
        std::memcpy(buffer.data(), data, len);
    }

private:
    int _read_head = 0;
};