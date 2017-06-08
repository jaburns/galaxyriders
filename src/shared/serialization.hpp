#pragma once

#include <vector>
#include <glm/vec3.hpp>

struct SerializationBuffer
{
    std::vector<unsigned char> buffer;

    void write_float(float& f);
    void write_vec3(glm::vec3& v);
};

class DeserializationBuffer
{
    std::vector<unsigned char> _buffer;
    int _read_head = 0;

public:

    DeserializationBuffer(const unsigned char *data, int len);
    float read_float();
    glm::vec3 read_vec3();
};