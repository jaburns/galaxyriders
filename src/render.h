#pragma once

#include <memory>
#include "glfw.h"
#include "resources.h"
#include "world.h"

class Renderer
{
    GLFWwindow* window;
    std::unique_ptr<ShaderProgram> program;
    std::unique_ptr<Texture> texture;
    GLuint index_buffer;

public:
    Renderer();
    ~Renderer();

    GLFWwindow* raw_glfw_window() const { return window; }
    bool should_close_window() const { return glfwWindowShouldClose(window); }
    void render(const World& world);
};
