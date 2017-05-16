#pragma once

#include "glfw.h"
#include <memory>
#include <string>

class GLuintResource
{
protected:
    GLuint _id = 0;
public:
    operator GLuint() const { return _id; }
};

struct ShaderProgram : public GLuintResource
{
    ShaderProgram(const char *vert_path, const char *frag_path);
    ~ShaderProgram();
};

struct Texture : public GLuintResource
{
    Texture(const char *png_path);
    ~Texture();
};

struct Mesh
{

};
