#pragma once

#include <deque>
#include <cstdint>
#include <vector>
#include "fixed32.hpp"
#include "input.hpp"
#include "level.hpp"

struct World
{
    struct Player {
        fixed32::vec2 position;
        fixed32::vec2 velocity;
    };


    fixed32::vec3 camera_position;
    fixed32::vec3 camera_look;
    int frame_counter;
    Player player;


    World();
    World(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
    World lerp_to(const World& next, float t) const;

    World step(const InputState& input) const;

    static const BakedLevel BAKED_LEVEL;
};
