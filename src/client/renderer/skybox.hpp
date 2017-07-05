#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"

class SkyboxRenderer
{
    GLuint _vao;
    GLuint _vertex_buffer;

    std::unique_ptr<const ShaderProgram> _program;
    std::shared_ptr<const CubeMap> _cubemap;

    SkyboxRenderer(const SkyboxRenderer&) =delete;
    SkyboxRenderer& operator=(const SkyboxRenderer&) =delete;

public:
    SkyboxRenderer();
    ~SkyboxRenderer();

    void draw_once(const glm::mat4x4& view, const glm::mat4x4& projection);

    std::shared_ptr<const CubeMap> get_skybox_cubemap() { return _cubemap; }
};