#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../gl.hpp"
#include "../core.hpp"
#include "../../shared/level.hpp"
#include "../gen/wire_meshes.hpp"

GameRenderer::GameRenderer()
:   m_level_renderer(World::GLOBAL_LEVEL),
    m_player_renderer(),
    m_mouse_renderer(WireMeshes::CIRCLE)
{ }

void GameRenderer::render(const ClientState& state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto p = Core::get_perspective_matrix();
    const auto v = Core::get_view_matrix(state.camera_pos);

    const auto mouse_pos = Core::get_mouse_world_pos(state.camera_pos, state.last_input.mouse_pos, p, v);

    m_level_renderer.draw_once(state.debug_paused, mouse_pos, v, p, {0.0f, 0.0f, -0.01f});
    m_player_renderer.draw_once(v, p, state.world.player, state.player_anim);

    if (state.debug_paused) {
        m_mouse_renderer.use(v, p);
        m_mouse_renderer.draw(glm::vec3(mouse_pos, 0.01f), glm::vec3(1.0f), {1.0f, 0.0f, 0.0f});
        m_mouse_renderer.done();
    }
}
