#include "state.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

static constexpr float EDITMODE_CAMERA_SLIDE = 0.05f;
static constexpr float EDITMODE_CAMERA_ZOOM  = 1.05f;

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

static void step_camera(ClientState& state)
{
    const auto target_dist = 10.0f + 20.0f * glm::length(state.world.players.at(state.player_id).velocity);
    state.camera_pos.x = state.world.players.at(state.player_id).position.x;
    state.camera_pos.y = state.world.players.at(state.player_id).position.y;
    state.camera_pos.z += (target_dist - state.camera_pos.z) / 10.0f;
}

static void step_game_mode(ClientState& state, const InputState& input, bool single_step)
{
    const auto old_player = state.world.players.at(state.player_id);

    World::Input inp;
    inp.player_id = state.player_id;
    inp.old_input = state.last_input.player;
    inp.new_input = input.player;

    std::vector<World::Input> inps;
    inps.push_back(inp);

    state.world.step(inps);

    if (!single_step) {
        step_camera(state);
    }

    state.player_anims[state.player_id].step(old_player, state.world.players.at(state.player_id), input.player.left, input.player.right);
}

static void step_edit_mode(ClientState& state, const InputState& input)
{
    if (input.player.right)   state.camera_pos.x += EDITMODE_CAMERA_SLIDE * state.camera_pos.z;
    if (input.player.left)    state.camera_pos.x -= EDITMODE_CAMERA_SLIDE * state.camera_pos.z;
    if (input.player.up)      state.camera_pos.y += EDITMODE_CAMERA_SLIDE * state.camera_pos.z;
    if (input.player.down)    state.camera_pos.y -= EDITMODE_CAMERA_SLIDE * state.camera_pos.z;
    if (input.editmode_zoom_out) state.camera_pos.z *= EDITMODE_CAMERA_ZOOM;
    if (input.editmode_zoom_in)  state.camera_pos.z /= EDITMODE_CAMERA_ZOOM;

    if (input.mouse_click) {
        const auto& mouse_pos = Core::get_mouse_world_pos(state.camera_pos, input.mouse_pos);

        state.edit_mode.selected_handle = 0;
        state.edit_mode.selected_poly = 0;
        float min_dist2 = 1e20f;
        for (int32_t i = 0; i < LoadedLevel::get().polys.size(); ++i) {
            for (int32_t j = 0; j < LoadedLevel::get().polys[i].handles.size(); ++j) {
                const auto& h = LoadedLevel::get().polys[i].handles[j];
                const auto new_d2 = glm::distance2(mouse_pos, h.point);
                if (new_d2 < min_dist2) {
                    state.edit_mode.selected_handle = i;
                    state.edit_mode.selected_poly = j;
                    min_dist2 = new_d2;
                }
            }
        }

        LoadedLevel::get().polys[state.edit_mode.selected_handle].handles[state.edit_mode.selected_poly].point = mouse_pos;
        LoadedLevel::bake();
    }
}

void ClientState::step(const InputState& input)
{
    if (input.editmode_toggle && !last_input.editmode_toggle) {
        edit_mode.enabled = !edit_mode.enabled;
    }

    if (input.debug_toggle_wireframe && !last_input.debug_toggle_wireframe) {
        debug_wireframe_mode = !debug_wireframe_mode;
    }

    if (!edit_mode.enabled || (input.editmode_step && !last_input.editmode_step)) {
        step_game_mode(*this, input, edit_mode.enabled);
    } else {
        step_edit_mode(*this, input);
    }

    last_input = input;
}

void ClientState::step_with_world(const World& new_world, const PlayerInput& input)
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