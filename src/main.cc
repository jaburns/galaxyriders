#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include "deps/linmath.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "ecs.h"

using namespace std;

extern const vec3 teapot_vertices[531];
extern const vec3 teapot_normals[531];
extern const unsigned int teapot_indices[3072];
extern GLuint loadTexture(const string filename, int &width, int &height);

static const char* vertex_shader_text =
    "#version 150\n"

    "in vec3 vPos;\n"
    "in vec3 vNorm;\n"

    "out vec2 v_tex_coords;\n"
    "out vec3 v_normal;\n"

    "uniform mat4 model;\n"
    "uniform mat4 perspective;\n"

    "void main()\n"
    "{\n"
        "v_normal = transpose(inverse(mat3(model))) * vNorm;\n"
        "v_tex_coords = 0.01 * vPos.xy;\n"
        "gl_Position = perspective * model * vec4(vPos, 1.0);\n"
    "}\n";

static const char* fragment_shader_text =
    "#version 150\n"

    "in vec2 v_tex_coords;\n"
    "in vec3 v_normal;\n"

    "out vec4 color;\n"

    "const vec3 light = vec3(-1.0, 0.4, 0.9);\n"
    "uniform sampler2D tex;\n"

    "void main()\n"
    "{\n"
        "float brightness = dot(normalize(v_normal), normalize(light));\n"
        "color = (0.5 + 0.5 * brightness) * texture(tex, v_tex_coords);\n"
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

extern void ecs_test();

int main(void)
{
    ecs_test();
    return 0;


    GLFWwindow* window;

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
    glfwSwapInterval(0); // TODO set to 1 for vsync

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    GLuint program = compile_program(vertex_shader_text, fragment_shader_text);
    GLint model_location = glGetUniformLocation(program, "model");
    GLint perspective_location = glGetUniformLocation(program, "perspective");
    GLint texture_location = glGetUniformLocation(program, "tex");
    GLint vpos_location = glGetAttribLocation(program, "vPos");
    GLint vnorm_location = glGetAttribLocation(program, "vNorm");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_vertices), teapot_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    GLuint normal_buffer;
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapot_normals), teapot_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vnorm_location);
    glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapot_indices), teapot_indices, GL_STATIC_DRAW);

    int texWidth, texHeight;
    GLint texture = loadTexture("res/texture.png", texWidth, texHeight);

    float t = -0.5;

    glClearColor(0, 0, 1, 1);

    while (!glfwWindowShouldClose(window)) {
        t += 0.0008;
        if (t > 3.14159) {
            t = -3.14159;
        }

        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 m;
        mat4x4_translate(m, 0, 0, -2);
        mat4x4_scale_aniso(m, m, 0.01, 0.01, 0.01);
        mat4x4_rotate_Y(m, m, -t);

        mat4x4 p;
        mat4x4_perspective(p, 3.14159 / 3.0, width / (float)height, 1.0, 1024.0);

        glUseProgram(program);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat*) m);
        glUniformMatrix4fv(perspective_location, 1, GL_FALSE, (const GLfloat*) p);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texture_location, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glDrawElements(GL_TRIANGLES, 3072, GL_UNSIGNED_INT, (void*)0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
