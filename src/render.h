#pragma once

#include "glfw.h"
#include "resources.h"
#include "world.h"

class Renderer
{
    GLFWwindow* window;
    ShaderProgram program;
    Texture texture;
    GLuint index_buffer;

public:
    Renderer();
    ~Renderer();

    GLFWwindow* raw_glfw_window() const { return window; }
    bool should_close_window() const { return glfwWindowShouldClose(window); }
    void render(const World& world);
};
