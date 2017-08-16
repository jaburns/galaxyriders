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
        glm::vec2 position = { -0.0f, -12.0f };
        glm::vec2 velocity = { -0.0f, 0.0f };
        glm::vec2 ground_normal = { 0.0f, 1.0f };
        uint8_t grounded = 0;
        bool air_stomping = false;
    };

    int frame_counter = 0;
    Player player;

    World() { }
    World(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
    World lerp_to(const World& next, float t) const;

    void step(const SharedInputState& old_input, const SharedInputState& new_input);

    static const BakedLevel BAKED_LEVEL;
};
