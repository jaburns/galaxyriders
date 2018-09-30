#pragma once
#include "../../shared/global.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../gl.hpp"
#include "../resources.hpp"
#include "../../shared/lang_utils.hpp"

class WireRenderer : public NoCopy
{
    GLuint m_vao;
    GLuint m_vertex_buffer;
    GLsizei m_lines_size;
    
    const ShaderProgram m_program;

public:
    WireRenderer(const WireMesh& mesh);
    ~WireRenderer();

    void use(const glm::mat4x4& view, const glm::mat4x4& projection) const;
    void draw(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color) const;
    void done() const;
};