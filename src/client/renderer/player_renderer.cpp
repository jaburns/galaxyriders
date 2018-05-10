#include "player_renderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/rotate_vector.hpp>
#include "../gen/wire_meshes.hpp"

PlayerRenderer::PlayerRenderer()
:   m_sprite_renderer("guy", glm::vec2(0.5f, -1.0f)),
    m_wire_sphere_renderer(WireMeshes::CIRCLE)
{ }

void PlayerRenderer::draw_once(
    const glm::mat4x4& view, const glm::mat4x4& projection,
    const WorldState::Player& player, const ClientState::PlayerAnimation& anim
) const {
    const auto pos3 = glm::vec3(player.position, 0.0f);

//  m_wire_sphere_renderer.use(view, projection);
//  m_wire_sphere_renderer.draw(pos3, {0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f});
//  m_wire_sphere_renderer.done();

    const auto sprite_offset = glm::rotate(glm::vec2(0.0f, -0.57f), anim.radians);

    m_sprite_renderer.use(view, projection);
    m_sprite_renderer.draw(pos3 + glm::vec3(sprite_offset, 0.0f), anim.radians, 1.0f, anim.frame, anim.face_left);
    m_sprite_renderer.done();
}