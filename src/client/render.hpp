#pragma once

#include <memory>
#include "glfw.hpp"
#include "resources.hpp"
#include "renderer/teapot.hpp"
#include "renderer/skybox.hpp"
#include "../shared/world.hpp"

class Renderer
{
    GLFWwindow* window;
    std::unique_ptr<SkyboxRenderer> _skybox_renderer;
    std::unique_ptr<TeapotRenderer> _teapot_renderer;

    // TODO prevent copy of this class

public:
    Renderer();
    ~Renderer();

    GLFWwindow* raw_glfw_window() const { return window; }
    bool should_close_window() const { return glfwWindowShouldClose(window); }
    void render(const World& world);
};
