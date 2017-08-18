#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../gl.hpp"
#include "../resources.hpp"
#include "../../shared/level.hpp"
#include "../../shared/lang_utils.hpp"

class LevelMeshRenderer : public NoCopy
{
public:
    struct Mesh
    {
        std::vector<glm::vec2> vertices;
        std::vector<glm::vec3> surface_info;
        std::vector<uint32_t> indices;
    };

private:
    GLuint m_vao = 0;
    GLuint m_vertex_buffer = 0;
    GLuint m_surface_pos_buffer = 0;
    GLuint m_index_buffer = 0;

    const Mesh m_mesh;
    const ShaderProgram m_program;
    const Texture m_noise_texture;

public:
    LevelMeshRenderer(const Level& level);
    ~LevelMeshRenderer();

    void draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const;
};