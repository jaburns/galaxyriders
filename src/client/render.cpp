#include "render.hpp"

#include <utility>
#include <stdlib.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>

#include "models/teapot.hpp"

static GLfloat skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,

    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
};



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
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(0.16f, 0.17f, 0.18f, 1.0f);

    program = std::make_unique<ShaderProgram>("res/shaders/main.vert", "res/shaders/main.frag");;
    texture = std::make_unique<Texture>("res/texture.png");
    skyboxShader = std::make_unique<ShaderProgram>("res/shaders/skybox.vert", "res/shaders/skybox.frag");;
    skyboxTexture = std::make_unique<CubeMap>(
        "res/hw_crater/craterlake_rt.png",
        "res/hw_crater/craterlake_lf.png",
        "res/hw_crater/craterlake_up.png",
        "res/hw_crater/craterlake_dn.png",
        "res/hw_crater/craterlake_bk.png",
        "res/hw_crater/craterlake_ft.png"
    );

    // Setup teapot VAO
//  GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_vertices), teapot_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_normals), teapot_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*program, "normal"));
    glVertexAttribPointer(glGetAttribLocation(*program, "normal"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapot_indices), teapot_indices, GL_STATIC_DRAW);

    // Setup skybox VAO
//  GLuint skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    GLuint skyboxVBO;
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*skyboxShader, "position"));
    glVertexAttribPointer(glGetAttribLocation(*skyboxShader, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *skyboxTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *texture);

    {
        auto new_v = glm::mat4(glm::mat3(v));

        glDepthMask(GL_FALSE);

        glUseProgram(*skyboxShader);
        glUniformMatrix4fv(glGetUniformLocation(*skyboxShader, "view"), 1, GL_FALSE, glm::value_ptr(new_v));
        glUniformMatrix4fv(glGetUniformLocation(*skyboxShader, "projection"), 1, GL_FALSE, glm::value_ptr(p));
        glUniform1i(glGetUniformLocation(*skyboxShader, "skybox"), 0);

        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
    }

    {
        glUseProgram(*program);
        glUniform3fv(glGetUniformLocation(*program, "camera_pos"), 1, glm::value_ptr(world.camera_position));
        glUniformMatrix4fv(glGetUniformLocation(*program, "view"), 1, GL_FALSE, glm::value_ptr(v));
        glUniformMatrix4fv(glGetUniformLocation(*program, "projection"), 1, GL_FALSE, glm::value_ptr(p));
        glUniform1i(glGetUniformLocation(*program, "skybox"), 0);
        glUniform1i(glGetUniformLocation(*program, "surf_texture"), 1);

        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

        for (auto tp : world.teapots) {
            auto m = glm::scale(glm::translate(glm::mat4(1.0f), tp.transform.position), tp.transform.scale) 
                * glm::mat4_cast(tp.transform.rotation);

            glUniformMatrix4fv(glGetUniformLocation(*program, "model"), 1, GL_FALSE, glm::value_ptr(m));

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
