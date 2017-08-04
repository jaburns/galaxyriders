#include "render.hpp"

#include <utility>
#include <stdlib.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gen/wire_meshes.hpp"
#include "../shared/level.hpp"
#include "readinput.hpp"
#include "palette.hpp"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    exit(EXIT_FAILURE);
}

Renderer::Renderer()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//  glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    m_window = glfwCreateWindow(1280, 720, "Galaxy Riders", NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // 1: vsync, 0: fast

#ifdef _WIN32
    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult) {
        printf("ERROR: %s\n", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(Palette::COLOR_SPACE.r, Palette::COLOR_SPACE.g, Palette::COLOR_SPACE.b, 1.0f);
    glLineWidth(9.0f);

    m_skybox_renderer = std::make_unique<SkyboxRenderer>();
    m_level_renderer = std::make_unique<LevelRenderer>(World::BAKED_LEVEL);
    m_player_renderer = std::make_unique<PlayerRenderer>();
}

void Renderer::render(const ClientState& state)
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 cam_pos =
        { state.world.player.position.x,
          state.world.player.position.y,
          state.camera_dist };

    const auto p = glm::perspective(3.14159f / 3.0f, width / (float)height, 0.1f, 1024.0f);
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

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

Renderer::~Renderer()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
