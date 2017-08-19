#include "level.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include "../core.hpp"
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

void LevelRenderer::draw_handle(const glm::vec2& mouse_pos, const Level::Handle& handle) const
{
    const auto dist2 = glm::distance2(mouse_pos, handle.point);

    glm::vec3 color;
    if (dist2 < 1.0f) {
        color = glm::vec3(1.0f, 1.0f, 1.0f);
    } else if (handle.quality > 0) {
        color = glm::vec3(1.0f, 0.0f, 1.0f);
    } else {
        color = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    m_handle_renderer.draw(glm::vec3(handle.point, 0.1f), glm::vec3(1.0f), color);
}

void LevelRenderer::draw_once(bool edit_mode, const glm::vec2& mouse_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const
{
    m_level_mesh_renderer.draw_once(view, projection, position);

    if (edit_mode) {
        m_handle_renderer.use(view, projection);

        for (const auto p : m_level.polys) {
            for (const auto h : p.handles) {
                draw_handle(mouse_pos, h);
            }
        }
        m_handle_renderer.done();
    }
}