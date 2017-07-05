#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"

class LineRenderer
{
    GLuint m_vao;
    GLuint m_vertex_buffer;
    std::unique_ptr<const ShaderProgram> m_program;

    LineRenderer(const LineRenderer&) =delete;
    LineRenderer& operator=(const LineRenderer&) =delete;

public:
    LineRenderer();
    ~LineRenderer();

    void use(const glm::mat4x4& view, const glm::mat4x4& projection);
    void draw(const glm::vec3& position);
    void done();
};