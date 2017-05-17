#pragma once

#include <memory>
#include "glfw.hpp"
#include "resources.hpp"
#include "world.hpp"

class Renderer
{
    GLFWwindow* window;
    std::unique_ptr<ShaderProgram> program;
    std::unique_ptr<Texture> texture;
    std::unique_ptr<ShaderProgram> skyboxShader;
    std::unique_ptr<CubeMap> skyboxTexture;
    GLuint index_buffer;

    GLuint vao, skyboxVAO;

public:
    Renderer();
    ~Renderer();

    GLFWwindow* raw_glfw_window() const { return window; }
    bool should_close_window() const { return glfwWindowShouldClose(window); }
    void render(const World& world);
};
