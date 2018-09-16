#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "level_mesh_renderer.hpp"
#include "wire_renderer.hpp"
#include "../resources.hpp"
#include "../../shared/lang_utils.hpp"
#include "../../editor/editor_state.hpp"

class LoadedLevelRenderer : public NoCopy
{
    const WireRenderer m_handle_renderer;
    LevelMeshRenderer m_level_mesh_renderer;

    uint32_t m_level_checksum;

    void draw_handle(const glm::vec2& mouse_pos, const Level::Handle& handle, EditorState::SelectedHandleState selected) const;

public:
    LoadedLevelRenderer();

    void draw_once(const EditorState& editor, const glm::vec2& mouse_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position);
};
