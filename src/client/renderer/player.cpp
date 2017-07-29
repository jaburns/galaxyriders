#include "player.hpp"
#include "../gen/wire_meshes.hpp"

PlayerRenderer::PlayerRenderer()
{
    m_sprite_renderer = std::make_unique<SpriteRenderer>();
    m_wire_sphere_renderer = std::make_unique<WireRenderer>(WireMeshes::CIRCLE);
}

void PlayerRenderer::draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec2& position)
{
    const auto pos3 = glm::vec3(position, 0.0f);

    m_wire_sphere_renderer->use(view, projection);
    m_wire_sphere_renderer->draw(pos3, {0.1f, 0.1f, 0.1f}, { 0.0f, 1.0f, 0.0f });
    m_wire_sphere_renderer->done();

    m_sprite_renderer->use(view, projection);
    m_sprite_renderer->draw(pos3 + glm::vec3(0.0f, -0.12f, 0.0f), 0.3f, 0);
    m_sprite_renderer->done();
}