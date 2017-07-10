#pragma once

#include <deque>
#include <cstdint>
#include <vector>
#include "fixed32.hpp"
#include "input.hpp"

struct World
{
    fixed32::vec3 camera_position;
    fixed32::vec3 camera_look;
    int frame_counter;

    World();
    World(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
    World lerp_to(const World& next, float t) const;

    World step(const InputState& input) const;
};
