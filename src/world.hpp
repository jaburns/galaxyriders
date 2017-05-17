#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "input.hpp"

struct Teapot
{
    glm::vec3 position;
    float spin;
};

struct World
{
    glm::vec3 camera_position;
    glm::vec3 camera_up;
    glm::vec3 camera_look;

    std::vector<Teapot> teapots;

    World();
    World step(InputState& input) const;
};
