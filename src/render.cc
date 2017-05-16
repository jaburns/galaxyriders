#include "render.h"

#include <utility>
#include <stdlib.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "models/teapot.h"

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

    window = glfwCreateWindow(1280, 720, "Hello world", NULL, NULL);
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
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    program = std::make_unique<ShaderProgram>("res/shaders/main.vert", "res/shaders/main.frag");;
    texture = std::make_unique<Texture>("res/texture.png");

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_vertices), teapot_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*program, "vPos"));
    glVertexAttribPointer(glGetAttribLocation(*program, "vPos"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_normals), teapot_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*program, "vNorm"));
    glVertexAttribPointer(glGetAttribLocation(*program, "vNorm"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapot_indices), teapot_indices, GL_STATIC_DRAW);

    glClearColor(0, 0, 0, 1);
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

    {
        glUseProgram(*program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glUniform1i(glGetUniformLocation(*program, "texture"), 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

        for (auto tp : world.teapots) {
            auto m = glm::rotate(
                glm::scale(
                    glm::translate(glm::mat4(1.0f), tp.position), 
                    glm::vec3(0.01f)
                ),
                tp.spin,
                { 0.0f, 1.0f, 0.0f }
            );
            auto mv = v * m;
            auto mvp = p * mv;

            glUniformMatrix4fv(glGetUniformLocation(*program, "mv"), 1, GL_FALSE, glm::value_ptr(mv));
            glUniformMatrix4fv(glGetUniformLocation(*program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

            glDrawElements(GL_TRIANGLES, sizeof(teapot_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);
        }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Renderer::~Renderer()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}