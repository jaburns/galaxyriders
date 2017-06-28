#include "teapot.hpp"
#include "teapot_mesh.hpp" 

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

TeapotRenderer::TeapotRenderer(std::shared_ptr<const CubeMap> skybox_cubemap)
{
    _skybox_cubemap = skybox_cubemap;
    _program = std::make_unique<const ShaderProgram>("res/shaders/main.vert", "res/shaders/main.frag");
    _texture = std::make_unique<const Texture>("res/texture.png");

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, teapot_mesh.vertices.size() * sizeof(glm::vec3), teapot_mesh.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, teapot_mesh.normals.size() * sizeof(glm::vec3), teapot_mesh.normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*_program, "normal"));
    glVertexAttribPointer(glGetAttribLocation(*_program, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    glGenBuffers(1, &_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, teapot_mesh.indices.size() * sizeof(unsigned int), teapot_mesh.indices.data(), GL_STATIC_DRAW);
}

void TeapotRenderer::use(const glm::vec3& camera_pos, const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *_skybox_cubemap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *_texture);

    glUseProgram(*_program);
    glUniform3fv(glGetUniformLocation(*_program, "camera_pos"), 1, glm::value_ptr(camera_pos));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(*_program, "skybox"), 0);
    glUniform1i(glGetUniformLocation(*_program, "surf_texture"), 1);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
}

void TeapotRenderer::draw(const Transform& transform)
{
    auto m = glm::scale(glm::translate(glm::mat4(1.0f), transform.position), transform.scale) 
        * glm::mat4_cast(transform.rotation);

    glUniformMatrix4fv(glGetUniformLocation(*_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, teapot_mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
}