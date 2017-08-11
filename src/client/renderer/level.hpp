#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../resources.hpp"
#include "../level_mesh.hpp"
#include "../../shared/lang_utils.hpp"

class LevelRenderer : public NoCopy
{
    GLuint m_vao = 0;
    GLuint m_vertex_buffer = 0;
    GLuint m_surface_pos_buffer = 0;
    GLuint m_index_buffer = 0;

    const LevelMesh m_mesh;
    const ShaderProgram m_program;
    const Texture m_noise_texture;
    const Texture m_ground_texture;

public:
    LevelRenderer() {};
    LevelRenderer(const BakedLevel& level);
    ~LevelRenderer();

    void draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const;
};