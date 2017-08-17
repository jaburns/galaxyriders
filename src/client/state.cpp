#include "state.hpp"

#include <glm/glm.hpp>

void ClientState::PlayerAnimation::step(const World::Player& old_player, const World::Player& new_player, bool move_left, bool move_right)
{
    if (new_player.grounded > 0) {
        radians = atan2f(new_player.ground_normal.y, new_player.ground_normal.x) - 3.141592654f/2.0f;
        mode = STANDING;
    } else {
        radians *= 0.8f;
    }

    if (old_player.grounded > 0 && new_player.grounded == 0) {
        mode = OLLIE;
        frame = 0;
    }

    if (!old_player.air_stomping && new_player.air_stomping) {
        mode = STOMPING;
        frame = 12;
    }

    if (move_left)  face_left = true;
    if (move_right) face_left = false;

    switch (mode) {
        case STANDING:
            frame = 0;
            break;
        case OLLIE:
            if (++frame > 8) {
                mode = FLIPPING;
            }
            break;
        case FLIPPING:
            if (++frame > 12) {
                frame -= 4;
            }
            break;
        case STOMPING:
            if (frame < 18) frame++;
            break;
    }
}

static void step_game_mode(ClientState& state, const InputState& input, bool single_step)
{
    const auto old_player = state.world.player;
    state.world.step(state.last_input.shared, input.shared);

    if (!single_step) {
        const auto target_dist = 10.0f + 20.0f * glm::length(state.world.player.velocity);
        state.camera_pos.x = state.world.player.position.x;
        state.camera_pos.y = state.world.player.position.y;
        state.camera_pos.z += (target_dist - state.camera_pos.z) / 10.0f;
    }

    state.player_anim.step(old_player, state.world.player, input.shared.left, input.shared.right);
}

static constexpr float DEBUG_CAMERA_SLIDE = 0.05f;
static constexpr float DEBUG_CAMERA_ZOOM  = 1.05f;

static void step_debug_mode(ClientState& state, const InputState& input)
{
    if (input.shared.right)   state.camera_pos.x += DEBUG_CAMERA_SLIDE * state.camera_pos.z;
    if (input.shared.left)    state.camera_pos.x -= DEBUG_CAMERA_SLIDE * state.camera_pos.z;
    if (input.shared.up)      state.camera_pos.y += DEBUG_CAMERA_SLIDE * state.camera_pos.z;
    if (input.shared.down)    state.camera_pos.y -= DEBUG_CAMERA_SLIDE * state.camera_pos.z;
    if (input.debug_zoom_out) state.camera_pos.z *= DEBUG_CAMERA_ZOOM;
    if (input.debug_zoom_in)  state.camera_pos.z /= DEBUG_CAMERA_ZOOM;
}

void ClientState::step(const InputState& input)
{
    if (input.debug_pause && !last_input.debug_pause) {
        debug_paused = !debug_paused;
    }

    if (!debug_paused || (input.debug_step && !last_input.debug_step)) {
        step_game_mode(*this, input, debug_paused);
    } else {
        step_debug_mode(*this, input);
    }

    last_input = input;
}

ClientState ClientState::lerp_to(const ClientState& next, float t) const
{
    auto new_state = next;
    new_state.world = this->world.lerp_to(next.world, t);
    new_state.camera_pos = glm::mix(camera_pos, next.camera_pos, t);
    new_state.player_anim.radians = glm::mix(player_anim.radians, next.player_anim.radians, t);
    return new_state;
}
