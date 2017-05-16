#pragma once

#include "glfw.h"
#include <string>

class ShaderProgram
{
    GLuint _id = 0;
public:
    operator GLuint() const { return _id; }

    ShaderProgram() { }
    ShaderProgram(const char *vert_path, const char *frag_path);
    ~ShaderProgram();

    ShaderProgram(ShaderProgram&& other) {
        _id = other._id;
        other._id = 0;
    }

    ShaderProgram& operator=(ShaderProgram&& other) {
        if (this != &other) {
            _id = other._id;
            other._id = 0;
        }
        return *this;
    }
};

class Texture
{
    GLuint _id = 0;
public:
    operator GLuint() const { return _id; }

    Texture() { }
    Texture(const char *png_path);
    ~Texture();

    Texture(Texture&& other) {
        _id = other._id;
        other._id = 0;
    }

    Texture& operator=(Texture&& other) {
        if (this != &other) {
            _id = other._id;
            other._id = 0;
        }
        return *this;
    }
};

struct Mesh
{

};
