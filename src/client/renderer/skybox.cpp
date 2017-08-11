#include "skybox.hpp"

#include <glm/gtc/type_ptr.hpp>

static constexpr GLfloat skybox_vertices[] = {
    -1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
};

SkyboxRenderer::SkyboxRenderer()
:   m_program("res/shaders/skybox.vert", "res/shaders/skybox.frag"),
    m_cubemap(
        "res/skybox_small/right.png", 
        "res/skybox_small/left.png", 
        "res/skybox_small/up.png", 
        "res/skybox_small/down.png", 
        "res/skybox_small/back.png", 
        "res/skybox_small/front.png"
    )
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(m_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

SkyboxRenderer::~SkyboxRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
}

void SkyboxRenderer::draw_once(const glm::mat4x4& view, const glm::mat4x4& projection) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

    glDepthFunc(GL_LEQUAL);

    const auto trunc_view = glm::mat4(glm::mat3(view));

    glUseProgram(m_program);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(trunc_view));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(m_program, "skybox"), 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}