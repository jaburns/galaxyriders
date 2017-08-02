#pragma once

#include <memory>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"
#include "../../shared/world.hpp"

struct SpriteFrame
{
    glm::vec2 offset;
    glm::vec2 scale;
    glm::vec4 sprite_source;
};

class SpriteRenderer
{
    GLuint m_vao;
    GLuint m_vertex_buffer;

    std::unique_ptr<const ShaderProgram> m_program;
    std::unique_ptr<const Texture> m_texture;
    std::vector<SpriteFrame> m_frames;

    const std::string m_sprite_name;
    const glm::vec2 m_origin;

    float m_aspect;
    glm::vec2 m_scaled_origin;

    SpriteRenderer(const SpriteRenderer&) =delete;
    SpriteRenderer& operator=(const SpriteRenderer&) =delete;

    void load_frames();

public:
    SpriteRenderer(const std::string& sprite_name, const glm::vec2& origin);
    ~SpriteRenderer();

    void use(const glm::mat4x4& view, const glm::mat4x4& projection);
    void draw(const glm::vec3& position, float rotation_degs, float scale, int frame);
    void done();
};
