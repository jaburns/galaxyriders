#include "client_state.hpp"

#include <unordered_map>
#include <algorithm>
#include <glm/glm.hpp>

#include "../shared/logger.hpp"

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
    if (state.local_player_ids.size() == 1)
    {
        const auto player_id = state.local_player_ids[0].player_id;
        const auto target_dist = 20.0f + 30.0f * glm::length(state.world.players.at(player_id).velocity);
        state.camera_pos.x = state.world.players.at(player_id).position.x;
        state.camera_pos.y = state.world.players.at(player_id).position.y;
        state.camera_pos.z += (target_dist - state.camera_pos.z) / 10.0f;
    }
    else if (state.local_player_ids.size() == 2)
    {
        const auto a = state.local_player_ids[0].player_id;
        const auto b = state.local_player_ids[1].player_id;

        const float dist = glm::length(state.world.players.at(a).position - state.world.players.at(b).position);

        state.camera_pos.x = (state.world.players.at(a).position.x + state.world.players.at(b).position.x) / 2.0f;
        state.camera_pos.y = (state.world.players.at(a).position.y + state.world.players.at(b).position.y) / 2.0f;
        state.camera_pos.z = dist < 10 ? 10 : dist;
    }
}

static void handle_local_player_join(ClientState& state, const InputState& input)
{
    for (int i = 0, max = input.player_sources.size(); i < max; ++i)
    {
        if (!input.player_sources[i].join) continue;
        
        const auto& ids = state.local_player_ids;
        const bool input_already_used = ids.end() != std::find_if(
            ids.begin(), ids.end(), 
            [=](const auto& x) { return x.input_source_id == i; }
        );

        if (!input_already_used)
        {
            state.local_player_ids.push_back({ i, i });
            state.world.players[i];
        }
    }
}

void ClientState::step(const InputState& input, bool single_step)
{
    handle_local_player_join(*this, input);

    const auto old_players = world.players;

    std::vector<WorldState::Input> inps;

    for (int i = 0, max = local_player_ids.size(); i < max; ++i)
    {
        WorldState::Input inp;
        inp.player_id = local_player_ids[i].player_id;
        inp.old_input = last_input.player_sources[local_player_ids[i].input_source_id];
        inp.new_input = input.player_sources[local_player_ids[i].input_source_id];
        inps.push_back(inp);
    }

    world.step(inps);

    if (!single_step) step_camera(*this);

    for (int i = 0, max = local_player_ids.size(); i < max; ++i)
    {
        const auto id = local_player_ids[i];

        player_anims[id.player_id].step(
            old_players.at(id.player_id),
            world.players.at(id.player_id),
            input.player_sources[id.input_source_id].left,
            input.player_sources[id.input_source_id].right
        );
    }

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