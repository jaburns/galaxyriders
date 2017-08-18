#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../gl.hpp"
#include "../core.hpp"
#include "../../shared/level.hpp"
#include "../level_mesh.hpp"
#include "../gen/wire_meshes.hpp"

GameRenderer::GameRenderer()
:   m_skybox_renderer(),
    m_level_renderer(World::GLOBAL_LEVEL),
    m_player_renderer(),
    m_mouse_renderer(WireMeshes::CIRCLE)
{ }

void GameRenderer::render(const ClientState& state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto p = glm::perspective(3.14159f / 3.0f, Core::g_window_width / (float)Core::g_window_height, 0.1f, 1024.0f);
    const auto v = glm::translate(glm::lookAt(glm::vec3(0.0f), { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }), -state.camera_pos);

    const auto mouse_ray = Core::get_mouse_ray(Core::read_input_state().mouse_pos, p, v);
    const auto plane_normal = glm::vec3(0.0f, 0.0f, -1.0f);
    const auto plane_coord = glm::vec3(0.0f);
    const auto t = (glm::dot(plane_normal, plane_coord) - glm::dot(plane_normal, state.camera_pos)) / glm::dot(plane_normal, mouse_ray);
    const auto ray_pos = state.camera_pos + t * mouse_ray;

    m_level_renderer.draw_once(v, p, { 0.0f, 0.0f, -0.01f });
//  m_skybox_renderer.draw_once(v, p);
    m_player_renderer.draw_once(v, p, state.world.player, state.player_anim);

    m_mouse_renderer.use(v, p);
    m_mouse_renderer.draw({ray_pos.x, ray_pos.y, 0.01f}, glm::vec3(0.1f), {1.0f, 0.0f, 0.0f});
    m_mouse_renderer.done();
}
