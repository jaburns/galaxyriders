#pragma once

#include "gfx.hpp"
#include <memory>
#include <cstdint>
#include <vector>
#include <string>
#include <glm/vec3.hpp>


struct Mesh
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;

    Mesh() {}

    Mesh(const std::vector<glm::vec3> vertices, const std::vector<glm::vec3> normals, const std::vector<uint32_t> indices)
        : vertices(vertices), normals(normals), indices(indices) {}

    Mesh(int vertex_count, int normal_count, int index_count, const glm::vec3 vertices[], const glm::vec3 normals[], const uint32_t indices[])
        : vertices(vertices, vertices + vertex_count), normals(normals, normals + normal_count), indices(indices, indices + index_count) {}
};

struct WireMesh
{
    const float *points;
    size_t size;
};


class GLuintResource
{
    GLuintResource(const GLuintResource&) =delete;
    GLuintResource& operator=(const GLuintResource&) =delete;

protected:
    GLuint m_id = 0;

public:
    GLuintResource() {}
    operator GLuint() const { return m_id; }
};

struct ShaderProgram : public GLuintResource
{
    ShaderProgram(const std::string& vert_path, const std::string& frag_path);
    ~ShaderProgram();
};

struct Texture : public GLuintResource
{
    Texture(const std::string& png_path);
    ~Texture();
};

struct CubeMap : public GLuintResource
{
    CubeMap(const std::string& r, const std::string& l, const std::string& t, const std::string& bo, const std::string& ba, const std::string& f);
    ~CubeMap();
};
