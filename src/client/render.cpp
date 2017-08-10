#include "render.hpp"

#include <utility>
#include <stdlib.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gen/wire_meshes.hpp"
#include "../shared/level.hpp"
#include "readinput.hpp" // Only needed for as long as we're hacking the input directly from here across to the readinput module.
#include "palette.hpp"

Renderer::Renderer()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    m_window_width = 1280;
    m_window_height = 720;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
    SDL_GL_SetSwapInterval(1); // 1: vsync, 0: fast

    m_window = SDL_CreateWindow(
        "Galaxy Riders", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_window_width, m_window_height,
        SDL_WINDOW_OPENGL
    );

    SDL_SetWindowResizable(m_window, SDL_TRUE);

    m_context = SDL_GL_CreateContext(m_window);

#ifdef _WIN32
    glewExperimental = GL_TRUE;
    const auto glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        printf("ERROR: %s\n", glewGetErrorString(glewInitResult));
        exit(EXIT_FAILURE);
    }
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(Palette::COLOR_SPACE.r, Palette::COLOR_SPACE.g, Palette::COLOR_SPACE.b, 1.0f);
    glLineWidth(1.0f);

    m_skybox_renderer = std::make_unique<SkyboxRenderer>();
    m_level_renderer = std::make_unique<LevelRenderer>(World::BAKED_LEVEL);
    m_player_renderer = std::make_unique<PlayerRenderer>();
}

#include <iostream>

void Renderer::render(const ClientState& state)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 cam_pos =
        { state.world.player.position.x,
          state.world.player.position.y,
          state.camera_dist };

    const auto p = glm::perspective(3.14159f / 3.0f, m_window_width / (float)m_window_height, 0.1f, 1024.0f);
    const auto v = glm::translate(glm::lookAt(glm::vec3(0.0f), { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }), -cam_pos);

    const auto mouse_ray = Input::get_mouse_ray(Input::read_state().mouse_pos, p, v);
    const glm::vec3 plane_normal = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 plane_coord = glm::vec3(0.0f);
    float t = (glm::dot(plane_normal, plane_coord) - glm::dot(plane_normal, cam_pos)) / glm::dot(plane_normal, mouse_ray);
    const glm::vec3 ray_pos = cam_pos + t * mouse_ray;

    m_level_renderer->draw_once(v, p, { 0.0f, 0.0f, -0.01f });
    m_skybox_renderer->draw_once(v, p);
    m_player_renderer->draw_once(v, p, state.world.player, state.player_anim);
    //TODO draw something at ray_pos

    SDL_GL_SwapWindow(m_window);

    // TODO figure out how to share event queue between render and input modules
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_should_close_window = true;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    m_window_width = event.window.data1;
                    m_window_height = event.window.data2;
                    glViewport(0, 0, m_window_width, m_window_height);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_should_close_window = true;
                }
                Input::GLOBAL_key_callback(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                Input::GLOBAL_key_callback(event.key.keysym.sym, false);
                break;
        }
    }
}

Renderer::~Renderer()
{
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}