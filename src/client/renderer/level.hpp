#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"
#include "../../shared/level.hpp"

class LevelRenderer
{
    struct Mesh {
        std::vector<glm::vec3> vertices;
        std::vector<float> vdepths;
        std::vector<uint32_t> indices;
    };

    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLuint m_vdepth_buffer;
    GLuint m_index_buffer;

    Mesh m_mesh;
    std::unique_ptr<const ShaderProgram> m_program;

    static Mesh load_mesh(const BakedLevel& level);

    LevelRenderer(const LevelRenderer&) =delete;
    LevelRenderer& operator=(const LevelRenderer&) =delete;

public:
    LevelRenderer(const BakedLevel& level);
    ~LevelRenderer();

    void draw_once(const glm::vec3& camera_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position);
};
