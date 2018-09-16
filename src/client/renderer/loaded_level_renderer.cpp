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

void LoadedLevelRenderer::draw_handle(const glm::vec2& mouse_pos, const Level::Handle& handle, EditorState::SelectedHandleState selected) const
{
    const auto dist2 = glm::distance2(mouse_pos, handle.point);
    glm::vec3 color;

    switch (selected)
    {
        case EditorState::SelectedHandleState::Not: 
            color = handle.quality > 0
                ? glm::vec3(1.0f, 0.0f, 1.0f)
                : glm::vec3(1.0f, 1.0f, 1.0f);
            break;

        case EditorState::SelectedHandleState::Hovered:
            color = glm::vec3(0.0f, 1.0f, 0.0f);
            break;

        case EditorState::SelectedHandleState::Selected:
            color = glm::vec3(0.0f, 1.0f, 1.0f);
            break;
    }

    m_handle_renderer.draw(glm::vec3(handle.point, 0.1f), glm::vec3(1.0f), color);
}

void LoadedLevelRenderer::draw_once(const EditorState& editor, const glm::vec2& mouse_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position)
{
    if (editor.paused) {
        const auto new_checksum = LoadedLevel::get().checksum();
        if (new_checksum != m_level_checksum) {
            m_level_mesh_renderer.rebuild_mesh(LoadedLevel::get_baked());
        }
        m_level_checksum = new_checksum;
    }

    m_level_mesh_renderer.draw_once(view, projection, position, editor.wireframe);

    if (editor.paused) 
    {
        m_handle_renderer.use(view, projection);

        const auto polys = LoadedLevel::get().polys;

        for (int i = 0; i < polys.size(); ++i) 
        for (int j = 0; j < polys[i].handles.size(); ++j)
        {
            bool selected = i == editor.selected_level_handle.poly && j == editor.selected_level_handle.handle;

            draw_handle(mouse_pos, polys[i].handles[j], selected ? editor.selected_level_handle_state : EditorState::SelectedHandleState::Not);
        }

        m_handle_renderer.done();
    }
}
