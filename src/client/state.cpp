#include "state.hpp"

#include <glm/glm.hpp>

static const float MOVEMENT_SPEED = 5.0f;

ClientState ClientState::step(const InputState& input) const
{
    auto new_state = *this;

    if (input.debug_pause && !last_input.debug_pause) {
        new_state.debug_paused = !new_state.debug_paused;
    }

    if (!new_state.debug_paused || (input.debug_step && !last_input.debug_step)) {
        new_state.world = new_state.world.step(last_input.shared, input.shared);
    }

    new_state.last_input = input;

    return new_state;
}

ClientState ClientState::lerp_to(const ClientState& next, float t) const
{
    ClientState new_state = next;
    new_state.world = this->world.lerp_to(next.world, t);
    return new_state;
}
