#include "render.hpp"

#include <utility>
#include <stdlib.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gen/wire_meshes.hpp"

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

    window = glfwCreateWindow(1280, 720, "Galaxy Riders", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
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

    _skybox_renderer = std::make_unique<SkyboxRenderer>();
    _sprite_renderer = std::make_unique<SpriteRenderer>();
    _wire_sphere_renderer = std::make_unique<WireRenderer>(WireMeshes::BOX_VERTICES);
}

void Renderer::render(const World& world)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto p = glm::perspective(3.14159f / 3.0f, width / (float)height, 0.1f, 1024.0f);
    auto v = glm::translate(
        glm::lookAt(glm::vec3(0.0f), world.camera_look, world.camera_up),
        -world.camera_position
    );

    _wire_sphere_renderer->use(v, p);
    _wire_sphere_renderer->draw({ 2.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    _wire_sphere_renderer->done();

    _skybox_renderer->draw_once(v, p);

    _sprite_renderer->use(v, p);
    _sprite_renderer->draw({ 0.0f, 0.0f, 0.0f }, world.frame_counter);
    _sprite_renderer->done();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Renderer::~Renderer()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}