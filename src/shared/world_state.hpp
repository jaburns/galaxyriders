#pragma once

#include <deque>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "player_input.hpp"
#include "level.hpp"

struct WorldState
{
    struct Input
    {
        int32_t player_id;
        PlayerInput old_input;
        PlayerInput new_input;
    };

    struct Player 
    {
        glm::vec2 position = { -0.0f, -12.0f };
        glm::vec2 velocity = { -0.0f, 0.0f };
        glm::vec2 ground_normal = { 0.0f, 1.0f };
        uint8_t grounded = 0;
        bool air_stomping = false;
    };

    int frame_counter = 0;
    std::unordered_map<int32_t, Player> players;

    WorldState() { }
    WorldState(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
    WorldState lerp_to(const WorldState& next, float t) const;

    void step(const std::vector<Input>& inputs);
};


#ifdef _DEBUG

namespace Physics
{
    extern float DT;
    extern float GRAVITY;
    extern float RADIUS;
    extern float WALK_ACCEL;
    extern float PUMP_ACCEL;
    extern float MAX_RUN_SPEED;
    extern float TURN_AROUND_MULTIPLIER;
    extern float JUMP_SPEED;
    extern int LATE_JUMP_FRAMES;
}

#endif
