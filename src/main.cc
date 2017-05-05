#ifdef _WIN32
#   include <GL/glew.h>
#elif __APPLE__
#   define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#include "deps/linmath.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "math.h"
#include "ecs.h"
#include "deps/lodepng.h"

using namespace std;
using namespace ecs;

extern const vec3 teapot_vertices[531];
extern const vec3 teapot_normals[531];
extern const unsigned int teapot_indices[3072];

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static GLuint compile_from_file(const char *shader_path, GLenum shader_type)
{
    ifstream file(shader_path);
    stringstream buffer;
    buffer << file.rdbuf();
    string str = buffer.str();
    const char *cstr = str.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);

    return shader;
}

static GLuint compile_program(const char *vert_path, const char *frag_path)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, compile_from_file(vert_path, GL_VERTEX_SHADER));
    glAttachShader(program, compile_from_file(frag_path, GL_FRAGMENT_SHADER));
    glLinkProgram(program);

    return program;
}



struct Transform {
    vec3 position;
    float rotation;
};

struct RotationSystem : public System<Transform>
{
    void run(EntityContext &c, Transform &t)
    {
        t.rotation = glfwGetTime();
        t.position[1] = sinf(t.rotation + t.position[0]);
    }
};

class TeaSystem : public System<Transform>
{
    GLuint program;
    GLint model_location;
    GLint perspective_location;
    GLint texture;
    GLint texture_location;
    GLint index_buffer;

public:
    const GLfloat* p;

    TeaSystem(
        GLuint program,
        GLint model_location,
        GLint perspective_location,
        GLint texture,
        GLint texture_location,
        GLint index_buffer
    ) :   program(program)
        , model_location(model_location)
        , perspective_location(perspective_location)
        , texture(texture)
        , texture_location(texture_location)
        , index_buffer(index_buffer)
    { }

    void run(EntityContext &c, Transform &t)
    {
        mat4x4 m;
        mat4x4_translate(m, t.position[0], t.position[1], t.position[2]);
        mat4x4_scale_aniso(m, m, 0.01, 0.01, 0.01);
        mat4x4_rotate_Y(m, m, t.rotation);

        glUseProgram(program);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, (const GLfloat*)m);
        glUniformMatrix4fv(perspective_location, 1, GL_FALSE, p);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texture_location, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glDrawElements(GL_TRIANGLES, 3072, GL_UNSIGNED_INT, (void*)0);
    }
};

GLint loadPNGTexture(const char *path)
{
    // Load file and decode image.
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, path);

    // If there's an error, display it.
    if (error != 0)
    {
        std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        return 1;
    }

    //Now generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return texture;
}



int main(void)
{
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
    glfwSwapInterval(1); // TODO set to 1 for vsync

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

    GLuint program = compile_program("res/shaders/main.vert", "res/shaders/main.frag");
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

    cout << "LOADING" << endl;

//    int texWidth, texHeight;
    GLint texture = loadPNGTexture("res/texture.png");

    cout << "Loaded texture result: " << texture << endl;

    glClearColor(0, 0.5, 0, 1);

    World w;

    auto pot1 = w.create_entity();
    pot1.add_component(Transform{ { -1.0, 0.0, -3.0 } });

    auto pot2 = w.create_entity();
    pot2.add_component(Transform{ { 1.0, 1.0, -3.0 } });

    TeaSystem s(
        program,
        model_location,
        perspective_location,
        texture,
        texture_location,
        index_buffer
    );

    RotationSystem r;

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 p;
        mat4x4_perspective(p, 3.14159 / 3.0, width / (float)height, 1.0, 1024.0);
        s.p = (const GLfloat*)p;

        w.run_system(r);
        w.run_system(s);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
