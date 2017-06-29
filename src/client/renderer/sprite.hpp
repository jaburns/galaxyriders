#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"

class SpriteRenderer
{
    GLuint _vao;
    GLuint _vertex_buffer;

    std::unique_ptr<const ShaderProgram> _program;

    SpriteRenderer(const SpriteRenderer&) =delete;
    SpriteRenderer& operator=(const SpriteRenderer&) =delete;

public:
    SpriteRenderer();
    ~SpriteRenderer();

    void use(const glm::mat4x4& view, const glm::mat4x4& projection);
    void draw(const Transform& transform);
};
