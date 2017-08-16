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

void ClientState::step(const InputState& input)
{
    if (input.debug_pause && !last_input.debug_pause) {
        debug_paused = !debug_paused;

        if (debug_paused) {
            debug_camera_pos = glm::vec3(world.player.position, camera_dist);
        }
    }

    if (!debug_paused || (input.debug_step && !last_input.debug_step)) {
        const auto old_player = world.player;

        world.step(last_input.shared, input.shared);

        const auto target_dist = 10.0f + 20.0f * glm::length(world.player.velocity);
        camera_dist += (target_dist - camera_dist) / 10.0f;
        player_anim.step(old_player, world.player, input.shared.left, input.shared.right);
    } else {
        if (input.shared.right)   debug_camera_pos.x += 1.0;
        if (input.shared.left)    debug_camera_pos.x -= 1.0;
        if (input.shared.up)      debug_camera_pos.y += 1.0;
        if (input.shared.down)    debug_camera_pos.y -= 1.0;
        if (input.debug_zoom_out) debug_camera_pos.z += 1.0;
        if (input.debug_zoom_in)  debug_camera_pos.z -= 1.0;
    }

    last_input = input;
}

ClientState ClientState::lerp_to(const ClientState& next, float t) const
{
    auto new_state = next;
    new_state.world = this->world.lerp_to(next.world, t);
    new_state.camera_dist = glm::mix(camera_dist, next.camera_dist, t);
    new_state.player_anim.radians = glm::mix(player_anim.radians, next.player_anim.radians, t);
    new_state.debug_camera_pos = glm::mix(debug_camera_pos, next.debug_camera_pos, t);
    return new_state;
}
