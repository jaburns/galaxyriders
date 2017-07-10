#pragma once

#include <deque>
#include <vector>
#include "fixed32.hpp"
#include "input.hpp"

struct World
{
    fixed32::vec3 camera_position;
    fixed32::vec3 camera_look;
    int frame_counter;

    World();
    World(const unsigned char *serialized, int serialized_length);
    std::vector<unsigned char> serialize() const;
    World lerp_to(const World& next, float t) const;

    World step(const InputState& input) const;
};