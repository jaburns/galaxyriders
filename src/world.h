#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "input.h"

struct Teapot
{
    glm::vec3 position;
    float spin;
};

struct World
{
    glm::vec3 camera_position;
    glm::vec3 camera_up;

    std::vector<Teapot> teapots;

    World();
    World step(Input& input) const;
};
