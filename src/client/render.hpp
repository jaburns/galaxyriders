#pragma once

#include <memory>
#include "glfw.hpp"
#include "resources.hpp"
#include "renderer/skybox.hpp"
#include "renderer/sprite.hpp"
#include "renderer/wire.hpp"
#include "../shared/world.hpp"

class Renderer
{
    GLFWwindow* window;
    std::unique_ptr<SkyboxRenderer> _skybox_renderer;
    std::unique_ptr<SpriteRenderer> _sprite_renderer;
    std::unique_ptr<WireRenderer> _wire_sphere_renderer;

    Renderer(const Renderer&) =delete;
    Renderer& operator=(const Renderer&) =delete;

public:
    Renderer();
    ~Renderer();

    GLFWwindow* raw_glfw_window() const { return window; }
    bool should_close_window() const { return glfwWindowShouldClose(window); }
    void render(const World& world);
};