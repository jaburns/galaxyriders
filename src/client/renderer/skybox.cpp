#include "skybox.hpp"

#include <glm/gtc/type_ptr.hpp>

static GLfloat skybox_vertices[] = {
    -1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
};

SkyboxRenderer::SkyboxRenderer()
{
    _program = std::make_unique<const ShaderProgram>("res/shaders/skybox.vert", "res/shaders/skybox.frag");;
    _cubemap = std::make_unique<const CubeMap>(
        "res/hw_crater/craterlake_rt.png",
        "res/hw_crater/craterlake_lf.png",
        "res/hw_crater/craterlake_up.png",
        "res/hw_crater/craterlake_dn.png",
        "res/hw_crater/craterlake_bk.png",
        "res/hw_crater/craterlake_ft.png"
    );

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void SkyboxRenderer::draw(const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *_cubemap);

    glDepthFunc(GL_LEQUAL);

    glUseProgram(*_program);
    auto trunc_view = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "view"), 1, GL_FALSE, glm::value_ptr(trunc_view));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(*_program, "skybox"), 0);

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}