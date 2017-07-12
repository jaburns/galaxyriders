#include "render.hpp"

#include <utility>
#include <stdlib.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gen/wire_meshes.hpp"
#include "../shared/level.hpp"

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
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
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
    glClearColor(0.16f, 0.17f, 0.18f, 1.0f);
    glLineWidth(9.0f);

    std::vector<int32_t> raw_level = {1,12,0,100000,-100000,10,191000,-438000,10,502000,-432000,0,630000,-285000,10,726000,-177000,10,751000,-199000,0,935000,-284000,0,1036000,-339000,0,1249000,-339000,0,1249000,-712000,0,10000,-714000,0,12000,-100000};
    auto baked_level = BakedLevel::from_level(Level::from_data(raw_level));

    m_skybox_renderer = std::make_unique<SkyboxRenderer>();
    m_sprite_renderer = std::make_unique<SpriteRenderer>();
    m_wire_sphere_renderer = std::make_unique<WireRenderer>(WireMeshes::CUBE);
    m_level_renderer = std::make_unique<LevelRenderer>(baked_level);
}

void Renderer::render(const World& world)
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto p = glm::perspective(3.14159f / 3.0f, width / (float)height, 0.1f, 1024.0f);
    auto v = glm::translate(
        glm::lookAt(glm::vec3(0.0f), fixed32::to_float(world.camera_look), { 0.0f, 1.0f, 0.0f }),
        -fixed32::to_float(world.camera_position)
    );

    m_level_renderer->draw_once(v, p, { 0.0f, 0.0f, -0.01f });

    m_wire_sphere_renderer->use(v, p);
    m_wire_sphere_renderer->draw({ 2.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    m_wire_sphere_renderer->done();

    m_skybox_renderer->draw_once(v, p);

    m_sprite_renderer->use(v, p);
    m_sprite_renderer->draw({ 0.0f, 0.0f, 0.0f }, world.frame_counter);
    m_sprite_renderer->done();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

Renderer::~Renderer()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
