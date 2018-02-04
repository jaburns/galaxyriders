#pragma once

#include <glm/vec3.hpp>
#include <cstdint>
#include "../shared/world.hpp"
#include "core.hpp"

struct ClientState
{
    struct PlayerAnimation 
    {
        enum Mode 
        {
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

    struct EditModeState 
    {
        bool enabled = false;
        int32_t selected_poly = -1;
        int32_t selected_handle = -1;
    };

    bool debug_wireframe_mode = false;

    EditModeState edit_mode;
    glm::vec3 camera_pos = { 0.0f, 0.0f, 10.0f };

    InputState last_input;
    PlayerAnimation player_anim;
    World world;

    void step(const InputState& input);
    void step_with_world(const World& new_world, const PlayerInput& input);

    ClientState lerp_to(const ClientState& next, float t) const;
};
