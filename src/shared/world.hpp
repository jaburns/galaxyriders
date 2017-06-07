#pragma once

#include <deque>
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

    glm::quat parent_pot_tilt;
    std::deque<Teapot> teapots;

    int frame_counter = 0;
    float time_factor = 1.0f;

    World();
    World step(InputState& input) const;
};

struct SerializedWorld
{
    std::vector<unsigned char> buffer;

    SerializedWorld(const World& world);
}