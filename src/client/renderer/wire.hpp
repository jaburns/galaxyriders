#pragma once

#include <memory>
#include <array>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"

struct WireMesh
{
    const glm::vec3 *points;
    size_t size;
};

class WireRenderer
{
    GLuint m_vao;
    GLuint m_vertex_buffer;
    std::unique_ptr<const ShaderProgram> m_program;

    WireRenderer(const WireRenderer&) =delete;
    WireRenderer& operator=(const WireRenderer&) =delete;

public:
    WireRenderer(const WireMesh& mesh);
    ~WireRenderer();

    void use(const glm::mat4x4& view, const glm::mat4x4& projection);
    void draw(const glm::vec3& position, const glm::vec3& color);
    void done();

    static const WireMesh BOX_VERTICES;
};