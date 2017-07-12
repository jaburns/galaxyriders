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

    std::vector<int32_t> raw_level = {
1,39,0,2513391,-699569,0,-744998,-692615,0,-744978,395135,0,-632611,213719,0,-333733,298053,0,-122529,296830,10,-163955,-72445,10,-116144,-113727,0,-48257,-114477,0,279768,-114616,0,279867,-234166,0,337951,-233561,0,338033,-113404,0,476122,-113037,10,556942,-113859,10,686895,-154987,0,780993,-157845,0,923526,-157979,0,923392,-211170,0,1041765,-211222,0,1041609,-157443,0,1114039,-157893,0,1113337,-211483,0,1227506,-211522,0,1227572,-158165,0,1298585,-158171,0,1298673,-211331,0,1408308,-210926,0,1408032,-158145,0,1637855,-158007,10,1706510,-159331,10,1758950,-148144,0,1834564,-37471,0,1900967,58360,0,2010603,62630,10,2089841,-71369,19,2203698,-43970,0,2244698,159378,0,2514225,1610607
    };
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
