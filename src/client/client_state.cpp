#include "client_state.hpp"

#include <unordered_map>
#include <glm/glm.hpp>

void ClientState::PlayerAnimation::step(const WorldState::Player& old_player, const WorldState::Player& new_player, bool move_left, bool move_right)
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

static void step_camera(ClientState& state)
{
    const auto target_dist = 10.0f + 20.0f * glm::length(state.world.players.at(state.player_id).velocity);
    state.camera_pos.x = state.world.players.at(state.player_id).position.x;
    state.camera_pos.y = state.world.players.at(state.player_id).position.y;
    state.camera_pos.z += (target_dist - state.camera_pos.z) / 10.0f;
}

void ClientState::step(const InputState& input, bool single_step)
{
    const auto old_player = world.players.at(player_id);

    WorldState::Input inp;
    inp.player_id = player_id;
    inp.old_input = last_input.player;
    inp.new_input = input.player;

    std::vector<WorldState::Input> inps;
    inps.push_back(inp);

    world.step(inps);

    if (!single_step) {
        step_camera(*this);
    }

    player_anims[player_id].step(old_player, world.players.at(player_id), input.player.left, input.player.right);

    last_input = input;
}

void ClientState::step_with_world(const WorldState& new_world, const PlayerInput& input)
{
    for (auto& p : new_world.players) {
        const auto old_player = world.players.count(p.first) > 0 ? world.players.at(p.first) : p.second;
        player_anims[p.first].step(old_player, p.second, input.left, input.right);
    }

    world = new_world;
    step_camera(*this);
}

ClientState ClientState::lerp_to(const ClientState& next, float t) const
{
    auto new_state = next;
    new_state.world = this->world.lerp_to(next.world, t);
    new_state.camera_pos = glm::mix(camera_pos, next.camera_pos, t);

    for (auto& p : new_state.player_anims) {
        if (player_anims.count(p.first) < 1) continue;

        p.second.radians = glm::mix(player_anims.at(p.first).radians, p.second.radians, t);
    }

    return new_state;
}