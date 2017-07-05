#include "line.hpp"

#include <tinyxml2.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

static GLfloat line_vertices[] = {
     0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 
     1.0f, 1.0f, 0.0f,
     1.0f, 0.0f, 0.0f, 
     0.0f, 0.0f, 0.0f,
};

LineRenderer::LineRenderer()
{
    m_program = std::make_unique<const ShaderProgram>("res/shaders/line.vert", "res/shaders/line.frag");;

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), &line_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

LineRenderer::~LineRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
}

void LineRenderer::use(const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glUseProgram(*m_program);
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(m_vao);
    glLineWidth(2.0f);
}

void LineRenderer::draw(const glm::vec3& position)
{
    auto m = glm::translate(glm::mat4(1.0f), position);
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));
    glDrawArrays(GL_LINE_STRIP, 0, 5);
}

void LineRenderer::done()
{
    // nop
}