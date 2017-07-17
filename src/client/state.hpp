#pragma once

#include <glm/vec3.hpp>
#include "../shared/world.hpp"
#include "readinput.hpp"

struct ClientState
{
    glm::vec3 camera_position = { 0.0f, 0.0f, 3.0f };
    glm::vec3 camera_look = { 0.0f, 0.0f, -1.0f };
    bool debug_paused = true;

    InputState last_input;
    World world;

    ClientState step(const InputState& input) const;
    ClientState lerp_to(const ClientState& next, float t) const;
};
