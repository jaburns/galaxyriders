#pragma once

#include "deps/glfw.h"
#include <string>

class ShaderProgram 
{
    GLuint _id;
public:
    operator GLuint() const { return _id; }

    ShaderProgram(const char *vert_path, const char *frag_path);
    ~ShaderProgram();
};

class Texture
{
    GLuint _id;
public:
    operator GLuint() const { return _id; }

    Texture(const char *png_path);
    ~Texture();
};

struct Mesh
{

};