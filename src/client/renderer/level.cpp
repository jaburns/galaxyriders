#include "level.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../triangulator.hpp"
#include "../palette.hpp"

LevelRenderer::LevelRenderer(const BakedLevel& level)
:   m_mesh(level),
    m_program("res/shaders/level.vert", "res/shaders/level.frag"),
    m_noise_texture("res/textures/noise.png"),
    m_ground_texture("res/textures/ground.png")
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.vertices.size() * sizeof(glm::vec2), m_mesh.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(m_program, "position"), 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);

    glGenBuffers(1, &m_surface_pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_surface_pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.surface_pos.size() * sizeof(glm::vec2), m_mesh.surface_pos.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "surface_pos"));
    glVertexAttribPointer(glGetAttribLocation(m_program, "surface_pos"), 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);

    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.indices.size() * sizeof(uint32_t), m_mesh.indices.data(), GL_STATIC_DRAW);
}

LevelRenderer::~LevelRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_surface_pos_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}

void LevelRenderer::draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_noise_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_ground_texture);

    glUseProgram(m_program);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(m_program, "noise_texture"), 0);
    glUniform1i(glGetUniformLocation(m_program, "ground_texture"), 1);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    const auto m = glm::translate(glm::mat4(1.0f), position);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, m_mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
}