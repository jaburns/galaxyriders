#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"
#include "../../shared/world.hpp"

class TeapotRenderer
{
    GLuint _vao;
    std::unique_ptr<const ShaderProgram> _program;
    std::unique_ptr<const Texture> _texture;
    std::shared_ptr<const CubeMap> _skybox_cubemap;
    GLuint _index_buffer;

    TeapotRenderer(const TeapotRenderer&) =delete;
    TeapotRenderer& operator=(const TeapotRenderer&) =delete;

public:
    TeapotRenderer(std::shared_ptr<const CubeMap> skybox_cubemap);
    void use(const glm::vec3& camera_pos, const glm::mat4x4& view, const glm::mat4x4& projection);
    void draw(const Transform& transform);
};