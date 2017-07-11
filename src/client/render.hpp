#pragma once

#include <memory>
#include "glfw.hpp"
#include "resources.hpp"
#include "renderer/skybox.hpp"
#include "renderer/sprite.hpp"
#include "renderer/wire.hpp"
#include "renderer/level.hpp"
#include "../shared/world.hpp"

class Renderer
{
    GLFWwindow* m_window;
    std::unique_ptr<SkyboxRenderer> m_skybox_renderer;
    std::unique_ptr<SpriteRenderer> m_sprite_renderer;
    std::unique_ptr<WireRenderer> m_wire_sphere_renderer;
    std::unique_ptr<LevelRenderer> m_level_renderer;

    Renderer(const Renderer&) =delete;
    Renderer& operator=(const Renderer&) =delete;

public:
    Renderer();
    ~Renderer();

    GLFWwindow* raw_glfw_window() const { return m_window; }
    bool should_close_window() const { return glfwWindowShouldClose(m_window); }
    void render(const World& world);
};
