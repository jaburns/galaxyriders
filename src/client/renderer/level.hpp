#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "level_mesh.hpp"
#include "wire.hpp"
#include "../resources.hpp"
#include "../../shared/lang_utils.hpp"

class LevelRenderer : public NoCopy
{
    const Level m_level;

    const LevelMeshRenderer m_level_mesh_renderer;
    const WireRenderer m_handle_renderer;

    void draw_handle(const glm::vec2& mouse_pos, const Level::Handle& handle) const;

public:
    LevelRenderer(const Level& level);
    ~LevelRenderer();

    void draw_once(bool edit_mode, const glm::vec2& mouse_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const;
};