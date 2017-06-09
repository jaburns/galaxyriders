#pragma once

#include <glm/vec3.hpp>

struct InputState
{
    glm::vec3 look_dir = { 0.0f, 0.0f, -1.0f };
    glm::vec3 movement = { 0.0f, 0.0f, 0.0f };
    bool clicking = false;
};