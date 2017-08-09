#pragma once

#include <memory>
#include "glfw.hpp"
#include "resources.hpp"
#include "renderer/skybox.hpp"
#include "renderer/player.hpp"
#include "renderer/level.hpp"
#include "state.hpp"
#include "../shared/world.hpp"

class Renderer
{
    SDL_Window* m_window;
    SDL_GLContext m_context;
    std::unique_ptr<SkyboxRenderer> m_skybox_renderer;
    std::unique_ptr<LevelRenderer> m_level_renderer;
    std::unique_ptr<PlayerRenderer> m_player_renderer;

    Renderer(const Renderer&) =delete;
    Renderer& operator=(const Renderer&) =delete;

public:
    Renderer();
    ~Renderer();

    SDL_Window* raw_glfw_window() const { return m_window; }
    bool should_close_window() const { return false; } //glfwWindowShouldClose(m_window); }
    void render(const ClientState& state);
};
