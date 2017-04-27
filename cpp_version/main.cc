#include <GLFW/glfw3.h>
#include "deps/linmath.h"
#include <stdlib.h>
#include <stdio.h>


extern const vec3 teapot_vertices[531];
extern const vec3 teapot_normals[531];
extern const unsigned int teapot_indices[3072];

static const char* vertex_shader_text =
    "uniform mat4 model;\n"
    "uniform mat4 perspective;\n"

    "attribute vec3 vPos;\n"
    "varying vec3 color;\n"

    "void main()\n"
    "{\n"
        "gl_Position = perspective * model * vec4(vPos, 1.0);\n"
        "color = 0.01 * vPos; //vec3(1.0); //vPos;\n"
    "}\n";

static const char* fragment_shader_text =
    "varying vec3 color;\n"

    "void main()\n"
    "{\n"
        "gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static GLuint compile_program(const GLchar *vert, const GLchar *frag)
{
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vert, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &frag, NULL);
    glCompileShader(fragment_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(1280, 720, "Hello world", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_vertices), teapot_vertices, GL_STATIC_DRAW);

    GLuint program = compile_program(vertex_shader_text, fragment_shader_text);

    GLint model_location = glGetUniformLocation(program, "model");
    GLint perspective_location = glGetUniformLocation(program, "perspective");
    GLint vpos_location = glGetAttribLocation(program, "vPos");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 m;
        mat4x4_identity(m);
        mat4x4_translate(m, 0, 0, -2);
        mat4x4_scale_aniso(m, m, 0.01, 0.01, 0.01);
        mat4x4_rotate_Y(m, m, (float) glfwGetTime());

        mat4x4 p;
        mat4x4_perspective(p, 3.14159 / 3.0, width / (float)height, 1.0, 1024.0);

        glUseProgram(program);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat*) m);
        glUniformMatrix4fv(perspective_location, 1, GL_FALSE, (const GLfloat*) p);
        glDrawElements(GL_TRIANGLES, 3072, GL_UNSIGNED_INT, teapot_indices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
