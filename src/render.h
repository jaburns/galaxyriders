#pragma once

#include "deps/glfw.h"
#include "resources.h"
#include "world.h"

class Renderer
{
    GLFWwindow* window;
    ShaderProgram* program;
    Texture* texture;
    GLuint index_buffer;

public:
    Renderer(GLFWkeyfun key_callback);
    ~Renderer();

    bool should_close_window();
    void render(const World& world);
};