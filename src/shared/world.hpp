#pragma once

#include <deque>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "input.hpp"

struct World
{
    glm::vec3 camera_position;
    glm::vec3 camera_up;
    glm::vec3 camera_look;

    int frame_counter;

    World();
    World(const unsigned char *serialized, int serialized_length);
    std::vector<unsigned char> serialize() const;

    World step(const InputState& input) const;
    World lerp_to(const World& next, float t) const;
};