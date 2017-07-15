#pragma once

#include <deque>
#include <cstdint>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "input.hpp"
#include "level.hpp"

struct World
{
    struct Player {
        glm::vec2 position = { -0.1f, 0.0f };
        glm::vec2 velocity = { -0.1f, 0.1f };
    };

    int frame_counter = 0;
    Player player;

    World() { }
    World(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
    World lerp_to(const World& next, float t) const;
    World step(const SharedInputState& input) const;

    static const BakedLevel BAKED_LEVEL;
};
