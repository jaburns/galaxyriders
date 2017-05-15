#include "render.h"

#include <stdlib.h>
#include "models/teapot.h"


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    exit(EXIT_FAILURE);
}

Renderer::Renderer(GLFWkeyfun key_callback)
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

    glfwSetKeyCallback(window, key_callback);
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

    program = new ShaderProgram("res/shaders/main.vert", "res/shaders/main.frag");
    texture = new Texture("res/texture.png");

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
    float ratio;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4x4 p;
    mat4x4_perspective(p, 3.14159 / 3.0, width / (float)height, 1.0, 1024.0);

        mat4x4 m;
        mat4x4_translate(m,
           world.teapots[0].position[0],
           world.teapots[0].position[1],
           world.teapots[0].position[2]
        );

        mat4x4_scale_aniso(m, m, 0.01, 0.01, 0.01);
        mat4x4_rotate_Y(m, m, 0);

        glUseProgram(*program);
        glUniformMatrix4fv(glGetUniformLocation(*program, "model"), 1, GL_FALSE, (const GLfloat*)m);
        glUniformMatrix4fv(glGetUniformLocation(*program, "perspective"), 1, GL_FALSE, (const GLfloat*)p);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glUniform1i(glGetUniformLocation(*program, "texture"), 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glDrawElements(GL_TRIANGLES, sizeof(teapot_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool Renderer::should_close_window()
{
    return glfwWindowShouldClose(window);
}

Renderer::~Renderer()
{
    delete program;
    delete texture;

    glfwDestroyWindow(window);
    glfwTerminate();
}
