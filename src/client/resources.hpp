#pragma once
#include "../shared/global.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include "gl.hpp"
#include "../shared/lang_utils.hpp"

struct WireMesh
{
    const float *points;
    size_t size;
};

class GLuintResource : public NoCopy
{
protected:
    GLuint m_id = 0;

public:
    GLuintResource() {}
    operator GLuint() const { return m_id; }
};

struct ShaderProgram : public GLuintResource
{
    ShaderProgram() {}
    ShaderProgram(const std::string& vert_path, const std::string& frag_path);
    ~ShaderProgram();
};

struct Texture : public GLuintResource
{
    Texture() {}
    Texture(const std::string& png_path);
    ~Texture();
};

struct CubeMap : public GLuintResource
{
    CubeMap() {}
    CubeMap(const std::string& r, const std::string& l, const std::string& t, const std::string& bo, const std::string& ba, const std::string& f);
    ~CubeMap();
};
