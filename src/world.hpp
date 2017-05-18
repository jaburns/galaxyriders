#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "input.hpp"


struct Transform
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct Teapot
{
    Transform transform;
    Transform velocity;
};

struct World
{
    glm::vec3 camera_position;
    glm::vec3 camera_up;
    glm::vec3 camera_look;

    std::vector<Teapot> teapots;

    int frame_counter;

    World();
    World step(InputState& input) const;
};
