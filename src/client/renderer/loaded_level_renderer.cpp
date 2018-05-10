#include "loaded_level_renderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include "../core.hpp"
#include "../gen/wire_meshes.hpp"
#include "../palette.hpp"

LoadedLevelRenderer::LoadedLevelRenderer()
:   m_level_mesh_renderer(LoadedLevel::get_baked()),
    m_handle_renderer(WireMeshes::CUBE)
{
    m_level_checksum = LoadedLevel::get().checksum();
}

LoadedLevelRenderer::~LoadedLevelRenderer()
{
}

void LoadedLevelRenderer::draw_handle(const glm::vec2& mouse_pos, const Level::Handle& handle) const
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


void LoadedLevelRenderer::draw_once(bool edit_mode, bool wireframe, const glm::vec2& mouse_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position)
{
    if (edit_mode) {
        const auto new_checksum = LoadedLevel::get().checksum();
        if (new_checksum != m_level_checksum) {
            m_level_mesh_renderer.rebuild_mesh(LoadedLevel::get_baked());
        }
        m_level_checksum = new_checksum;
    }

    m_level_mesh_renderer.draw_once(view, projection, position, wireframe);

    if (edit_mode) {
        m_handle_renderer.use(view, projection);

        for (const auto p : LoadedLevel::get().polys) {
            for (const auto h : p.handles) {
                draw_handle(mouse_pos, h);
            }
        }
        m_handle_renderer.done();
    }
}
