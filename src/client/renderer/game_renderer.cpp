#include "game_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../gl.hpp"
#include "../core.hpp"
#include "../../shared/level.hpp"
#include "../gen/wire_meshes.hpp"

GameRenderer::GameRenderer()
:   m_level_renderer(),
    m_player_renderer()
{ }

void GameRenderer::render(const ClientState& state, const CoreView& core_view, const EditorState& editor_state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto p = core_view.get_perspective_matrix();
    const auto v = core_view.get_view_matrix(state.camera_pos);

    const auto mouse_pos = core_view.get_mouse_world_pos(state.camera_pos);

    m_level_renderer.draw_once(editor_state.paused, editor_state.wireframe, mouse_pos, v, p, {0.0f, 0.0f, -0.01f});

    for (const auto& pl : state.world.players) {
        m_player_renderer.draw_once(v, p, pl.second, state.player_anims.at(pl.first));
    }
}