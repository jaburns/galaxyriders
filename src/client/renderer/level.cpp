#include "level.hpp"

#include "../gen/wire_meshes.hpp"
#include "../palette.hpp"

LevelRenderer::LevelRenderer(const Level& level)
:   m_level(level),
    m_level_mesh_renderer(level),
    m_handle_renderer(WireMeshes::CUBE)
{
}

LevelRenderer::~LevelRenderer()
{
}

void LevelRenderer::draw_once(bool edit_mode, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const
{
    m_level_mesh_renderer.draw_once(view, projection, position);

    if (edit_mode) {
        m_handle_renderer.use(view, projection);
        for (const auto p : m_level.polys) {
            for (const auto h : p.handles) {
                const glm::vec3 color = h.quality > 0 
                    ? glm::vec3(1.0f, 0.0f, 1.0f) 
                    : glm::vec3(0.0f, 1.0f, 0.0f);
                m_handle_renderer.draw(glm::vec3(h.point, 0.1f), glm::vec3(1.0f), color);
            }
        }
        m_handle_renderer.done();
    }
}