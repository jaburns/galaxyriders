#include "sprite.hpp"

static GLfloat quad_vertices[] = {
    -1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f
};

SpriteRenderer::SpriteRenderer()
{
    _program = std::make_unique<const ShaderProgram>("res/shaders/sprite.vert", "res/shaders/sprite.frag");;

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

SpriteRenderer::~SpriteRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vertex_buffer);
}

void SpriteRenderer::use(const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glUseProgram(*_program);
    glUniformMatrix4fv(glGetUniformLocation(*_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(_vao);
}

void SpriteRenderer::draw(const Transform& transform)
{
    auto m = glm::scale(glm::translate(glm::mat4(1.0f), transform.position), transform.scale) 
        * glm::mat4_cast(transform.rotation);

    glUniformMatrix4fv(glGetUniformLocation(*_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawArrays(GL_TRIANGLES, 0, 2);
}