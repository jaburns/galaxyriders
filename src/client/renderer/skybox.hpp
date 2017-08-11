#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../../shared/lang_utils.hpp"
#include "../gl.hpp"
#include "../resources.hpp"

class SkyboxRenderer : public NoCopy
{
    GLuint m_vao;
    GLuint m_vertex_buffer;

    const ShaderProgram m_program;
    const CubeMap m_cubemap;

public:
    SkyboxRenderer();
    ~SkyboxRenderer();

    void draw_once(const glm::mat4x4& view, const glm::mat4x4& projection) const;
    const CubeMap& get_skybox_cubemap() const { return m_cubemap; }
};