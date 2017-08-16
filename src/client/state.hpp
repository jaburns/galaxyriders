#pragma once

#include <glm/vec3.hpp>
#include <cstdint>
#include "../shared/world.hpp"
#include "core.hpp"

struct ClientState
{
    struct PlayerAnimation {
        enum Mode {
            STANDING,
            OLLIE,
            FLIPPING,
            STOMPING
        };

        float radians = 0.0f;
        int32_t frame = 0;
        bool face_left = false;
        Mode mode = STANDING;

        void step(const World::Player& old_player, const World::Player& new_player, bool move_left, bool move_right);
    };

    bool debug_paused = false;
    glm::vec3 debug_camera_pos;

    float camera_dist = 10.0f;

    InputState last_input;
    PlayerAnimation player_anim;
    World world;

    void step(const InputState& input);
    ClientState lerp_to(const ClientState& next, float t) const;
};
