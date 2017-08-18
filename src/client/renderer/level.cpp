#include "level.hpp"

LevelRenderer::LevelRenderer(const Level& level)
:   m_level_mesh_renderer(level)
{
}

LevelRenderer::~LevelRenderer()
{
}

void LevelRenderer::draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const
{
    m_level_mesh_renderer.draw_once(view, projection, position);
}