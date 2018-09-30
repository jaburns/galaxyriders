#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>
#include "../shared/world_state.hpp"
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

        void step(const WorldState::Player& old_player, const WorldState::Player& new_player, bool move_left, bool move_right);
    };

    struct LocalPlayerID
    {
        int player_id;
        int input_source_id;
    };

    glm::vec3 camera_pos = { 0.0f, 0.0f, 10.0f };
    std::vector<LocalPlayerID> local_player_ids;
    InputState last_input;
    std::unordered_map<int32_t, PlayerAnimation> player_anims;
    WorldState world;

    void step(const InputState& input, bool single_step);
    void step_with_world(const WorldState& new_world, const PlayerInput& input);

    ClientState lerp_to(const ClientState& next, float t) const;
};
