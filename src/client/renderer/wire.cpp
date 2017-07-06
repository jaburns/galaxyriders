#include "wire.hpp"

#include <tinyxml2.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

WireRenderer::WireRenderer(const WireMesh& mesh)
{
    m_program = std::make_unique<const ShaderProgram>("res/shaders/wire.vert", "res/shaders/wire.frag");;

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.size, mesh.points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (void*)0);

    m_lines_size = mesh.size / sizeof(GLfloat) / 3;
}

WireRenderer::~WireRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
}

void WireRenderer::use(const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glUseProgram(*m_program);
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(m_vao);
}

void WireRenderer::draw(const glm::vec3& position, const glm::vec3& color)
{
    auto m = glm::translate(glm::mat4(1.0f), position);
    glUniform3fv(glGetUniformLocation(*m_program, "line_color"), 1, glm::value_ptr(color));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));
    glDrawArrays(GL_LINES, 0, m_lines_size);
}

void WireRenderer::done()
{
    // nop
}