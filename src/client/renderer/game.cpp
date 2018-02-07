#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../gl.hpp"
#include "../core.hpp"
#include "../../shared/level.hpp"
#include "../gen/wire_meshes.hpp"

GameRenderer::GameRenderer()
:   m_level_renderer(),
    m_player_renderer()
{ }

void GameRenderer::render(const ClientState& state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto p = Core::get_perspective_matrix();
    const auto v = Core::get_view_matrix(state.camera_pos);

    const auto mouse_pos = Core::get_mouse_world_pos(state.camera_pos, state.last_input.mouse_pos);

    m_level_renderer.draw_once(state.edit_mode.enabled, state.debug_wireframe_mode, mouse_pos, v, p, {0.0f, 0.0f, -0.01f});

    for (const auto& pl : state.world.players) {
        m_player_renderer.draw_once(v, p, pl.second, state.player_anims.at(pl.first));
    }
}
