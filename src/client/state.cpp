#include "state.hpp"

#include <glm/glm.hpp>

static const float MOVEMENT_SPEED = 5.0f;

static bool clicked = false;

ClientState ClientState::step(const InputState& input) const
{
    auto new_state = *this;

    if (input.clicking) {
        if (!clicked) {
            new_state.world = new_state.world.step(input.shared);
        }
        clicked = true;
    } else {
        clicked = false;
    }

    new_state.camera_position += MOVEMENT_SPEED * input.movement;
    new_state.camera_look = input.look_dir;

    return new_state;
}

ClientState ClientState::lerp_to(const ClientState& next, float t) const
{
    ClientState new_state = *this;
    new_state.camera_position = glm::mix(new_state.camera_position, next.camera_position, t);
    new_state.camera_look = glm::mix(new_state.camera_look, next.camera_look, t);
    new_state.world = this->world.lerp_to(next.world, t);
    return new_state;
}
