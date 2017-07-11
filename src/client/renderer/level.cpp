#include "level.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

LevelRenderer::LevelRenderer(const BakedLevel& level)
{
    m_program = std::make_unique<const ShaderProgram>("res/shaders/level.vert", "res/shaders/level.frag");
    m_mesh = load_mesh(level);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.vertices.size() * sizeof(glm::vec3), m_mesh.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    glGenBuffers(1, &m_vdepth_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vdepth_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.vdepths.size() * sizeof(float), m_mesh.vdepths.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "normal"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.indices.size() * sizeof(uint32_t), m_mesh.indices.data(), GL_STATIC_DRAW);
}

LevelRenderer::~LevelRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_vdepth_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}

LevelRenderer::Mesh LevelRenderer::load_mesh(const BakedLevel& level)
{
    LevelRenderer::Mesh mesh;

    // TODO triangulate polys

    return mesh;
}

void LevelRenderer::draw_once(const glm::vec3& camera_pos, const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position)
{
    glUseProgram(*m_program);
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    auto m = glm::translate(glm::mat4(1.0f), position);

    glUniformMatrix4fv(glGetUniformLocation(*m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, m_mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
}
