#pragma once

#include <glm/vec3.hpp>
#include <cstdint>
#include "../shared/world.hpp"
#include "readinput.hpp"

struct ClientState
{
    struct PlayerAnimation {
        float radians = 0.0f;
        int32_t frame = 0;

        PlayerAnimation step(const World::Player& old_player, const World::Player& new_player) const;
    };

    bool debug_paused = true;

    float camera_dist = 10.0f;

    InputState last_input;
    PlayerAnimation player_anim;
    World world;

    ClientState step(const InputState& input) const;
    ClientState lerp_to(const ClientState& next, float t) const;
};
