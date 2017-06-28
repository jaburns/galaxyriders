#pragma once

#include "glfw.hpp"
#include <memory>
#include <vector>
#include <string>
#include <glm/vec3.hpp>

struct Mesh
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    Mesh() {}

    Mesh(const std::vector<glm::vec3> vertices, const std::vector<glm::vec3> normals, const std::vector<unsigned int> indices)
        : vertices(vertices), normals(normals), indices(indices) {}

    Mesh(int vertex_count, int normal_count, int index_count, const glm::vec3 vertices[], const glm::vec3 normals[], const unsigned int indices[])
        : vertices(vertices, vertices + vertex_count), normals(normals, normals + normal_count), indices(indices, indices + index_count) {}
};

class GLuintResource
{
    GLuintResource(const GLuintResource&) =delete;
    GLuintResource& operator=(const GLuintResource&) =delete;

protected:
    GLuint _id = 0;

public:
    GLuintResource() {}
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

struct CubeMap : public GLuintResource
{
    CubeMap(const char *r, const char *l, const char *t, const char *bo, const char *ba, const char *f);
    ~CubeMap();
};