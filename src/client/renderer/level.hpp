#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "level_mesh.hpp"
#include "../resources.hpp"
#include "../../shared/lang_utils.hpp"

class LevelRenderer : public NoCopy
{
    const LevelMeshRenderer m_level_mesh_renderer;

public:
    LevelRenderer(const Level& level);
    ~LevelRenderer();

    void draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const;
};