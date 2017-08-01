#pragma once

#include <glm/vec3.hpp>
#include "../shared/world.hpp"
#include "readinput.hpp"

struct ClientState
{
    bool debug_paused = true;

    float camera_dist = 10.0f;

    InputState last_input;
    World world;

    ClientState step(const InputState& input) const;
    ClientState lerp_to(const ClientState& next, float t) const;
};
