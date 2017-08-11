#include "game.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../../shared/level.hpp"
#include "../level_mesh.hpp"

GameRenderer::GameRenderer()
:   m_skybox_renderer(),
    m_level_renderer(World::BAKED_LEVEL),
    m_player_renderer()
{ }

void GameRenderer::render(const ClientState& state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto cam_pos = glm::vec3(
        state.world.player.position.x,
        state.world.player.position.y,
        state.camera_dist
    );

    const auto p = glm::perspective(3.14159f / 3.0f, Gfx::g_window_width / (float)Gfx::g_window_height, 0.1f, 1024.0f);
    const auto v = glm::translate(glm::lookAt(glm::vec3(0.0f), { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }), -cam_pos);

    //TODO draw something at ray_pos
    //  const auto mouse_ray = Input::get_mouse_ray(Input::read_state().mouse_pos, p, v);
    //  const auto plane_normal = glm::vec3(0.0f, 0.0f, -1.0f);
    //  const auto plane_coord = glm::vec3(0.0f);
    //  const auto t = (glm::dot(plane_normal, plane_coord) - glm::dot(plane_normal, cam_pos)) / glm::dot(plane_normal, mouse_ray);
    //  const auto ray_pos = cam_pos + t * mouse_ray;

    m_level_renderer.draw_once(v, p, { 0.0f, 0.0f, -0.01f });
    m_skybox_renderer.draw_once(v, p);
    m_player_renderer.draw_once(v, p, state.world.player, state.player_anim);
}