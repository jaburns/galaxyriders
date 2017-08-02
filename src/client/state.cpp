#include "state.hpp"

#include <glm/glm.hpp>

static const float MOVEMENT_SPEED = 5.0f;

#include <iostream>
#define DLOG(x) (std::cout << (x) << std::endl)

ClientState::PlayerAnimation ClientState::PlayerAnimation::step(const World::Player& old_player, const World::Player& new_player) const
{
    auto new_anim = *this;

    if (new_player.grounded > 0) {
        new_anim.radians = atan2f(new_player.ground_normal.y, new_player.ground_normal.x) - 3.141592654f/2.0f;
        new_anim.frame = 0;
    } else {
        new_anim.radians *= 0.8f;
    }

    if (old_player.grounded > 0 && new_player.grounded == 0) {
        DLOG("Ollied!");
    }

    return new_anim;
}

ClientState ClientState::step(const InputState& input) const
{
    auto new_state = *this;

    if (input.debug_pause && !last_input.debug_pause) {
        new_state.debug_paused = !new_state.debug_paused;
    }

    if (!new_state.debug_paused || (input.debug_step && !last_input.debug_step)) {
        new_state.world = new_state.world.step(last_input.shared, input.shared);
    }

    float target_dist = 10.0f + 20.0f * glm::length(new_state.world.player.velocity);
    new_state.camera_dist += (target_dist - new_state.camera_dist) / 10.0f;

    new_state.player_anim = new_state.player_anim.step(world.player, new_state.world.player);

    new_state.last_input = input;

    return new_state;
}

ClientState ClientState::lerp_to(const ClientState& next, float t) const
{
    ClientState new_state = next;
    new_state.world = this->world.lerp_to(next.world, t);
    new_state.camera_dist = glm::mix(camera_dist, next.camera_dist, t);
    new_state.player_anim.radians = glm::mix(player_anim.radians, next.player_anim.radians, t);
    return new_state;
}
