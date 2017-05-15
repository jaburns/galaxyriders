#pragma once

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

struct RawInput
{
    bool key_w = false;
    bool key_s = false;
    bool key_a = false;
    bool key_d = false;
};

struct Input
{
    glm::vec3 movement = { 0.0f, 0.0f, 0.0f };

    Input(const RawInput& raw)
    {
        bool not_zero = false;
        if (raw.key_w) { movement.z = -1.0f; not_zero = true; }
        if (raw.key_s) { movement.z =  1.0f; not_zero = true; }
        if (raw.key_a) { movement.x = -1.0f; not_zero = true; }
        if (raw.key_d) { movement.x =  1.0f; not_zero = true; }

        if (not_zero) {
            movement = 0.1f * glm::normalize(movement);
        }
    }
};
