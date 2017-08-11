#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "../gfx.hpp"
#include "../resources.hpp"
#include "../../shared/lang_utils.hpp"

class SpriteRenderer : public NoCopy
{
    struct SpriteFrame
    {
        glm::vec2 offset;
        glm::vec2 scale;
        glm::vec4 sprite_source;
    };

    GLuint m_vao;
    GLuint m_vertex_buffer;

    const ShaderProgram m_program;
    const Texture m_texture;

    const std::string m_sprite_name;
    const glm::vec2 m_origin;

    std::vector<SpriteFrame> m_frames;

    float m_aspect;
    glm::vec2 m_scaled_origin;

    SpriteRenderer(const SpriteRenderer&) =delete;
    SpriteRenderer& operator=(const SpriteRenderer&) =delete;

    void load_frames();

public:
    SpriteRenderer(const std::string& sprite_name, const glm::vec2& origin);
    ~SpriteRenderer();

    void use(const glm::mat4x4& view, const glm::mat4x4& projection) const;
    void draw(const glm::vec3& position, float rotation_rads, float scale, int frame, bool flipx = false) const;
    void done() const;
};