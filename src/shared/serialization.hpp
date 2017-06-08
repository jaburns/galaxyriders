#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct SerializationBuffer
{
    std::vector<unsigned char> buffer;

    template<typename T> 
    void write32(const T& val);

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
    void read32(T& val);

    void read_vec3(glm::vec3& v);
    void read_quat(glm::quat& q);
};