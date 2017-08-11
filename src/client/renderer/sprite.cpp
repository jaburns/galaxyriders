#include "sprite.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <tinyxml2.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../gl.hpp"
#include "../palette.hpp"

static constexpr GLfloat quad_vertices[] = {
     0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     1.0f, 1.0f, 0.0f
};

SpriteRenderer::SpriteRenderer(const std::string& sprite_name, const glm::vec2& origin)
:   m_sprite_name(sprite_name), 
    m_origin(origin),
    m_program("res/shaders/sprite.vert", "res/shaders/sprite.frag"),
    m_texture("res/sprites/" + sprite_name + ".sdf.png")
{
    load_frames();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(m_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void SpriteRenderer::load_frames()
{
    using namespace tinyxml2;
    constexpr auto SPRITE_SHEET_SIZE = 4096.0f;

    SpriteFrame new_frame;
    XMLDocument doc;
    doc.LoadFile(("res/sprites/" + m_sprite_name + ".xml").c_str());
    bool first_iter = true;

    const auto *atlas = doc.FirstChildElement();
    for (const auto *child = atlas->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        const auto sprite_x = child->FloatAttribute("x");
        const auto sprite_y = child->FloatAttribute("y");
        const auto sprite_width = child->FloatAttribute("width");
        const auto sprite_height = child->FloatAttribute("height");
        const auto frame_offset_x = child->FloatAttribute("frameX");
        const auto frame_offset_y = child->FloatAttribute("frameY");
        const auto frame_width = child->FloatAttribute("frameWidth");
        const auto frame_height = child->FloatAttribute("frameHeight");

        new_frame.sprite_source.x = sprite_x / SPRITE_SHEET_SIZE;
        new_frame.sprite_source.y = sprite_y / SPRITE_SHEET_SIZE;
        new_frame.sprite_source.z = new_frame.sprite_source.x + sprite_width / SPRITE_SHEET_SIZE;
        new_frame.sprite_source.w = new_frame.sprite_source.y + sprite_height / SPRITE_SHEET_SIZE;

        new_frame.scale.x = sprite_width / frame_width;
        new_frame.scale.y = sprite_height / frame_height;

        new_frame.offset.x = -frame_offset_x / frame_height; // Sprite frame is one world unit tall.
        new_frame.offset.y = -(frame_offset_y + sprite_height) / frame_height;

        m_frames.push_back(new_frame);

        if (first_iter) {
            m_aspect = frame_width / frame_height;
            m_scaled_origin = glm::vec2(m_origin.x * m_aspect, m_origin.y);
            first_iter = false;
        }
    }
}

SpriteRenderer::~SpriteRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
}

void SpriteRenderer::use(const glm::mat4x4& view, const glm::mat4x4& projection) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glUseProgram(m_program);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(m_program, "main_color"), 1, glm::value_ptr(Palette::COLOR_LIFE));
    glUniform1i(glGetUniformLocation(m_program, "sprite_texture"), 0);

    glBindVertexArray(m_vao);

    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SpriteRenderer::draw(const glm::vec3& position, float rotation_rads, float scale, int frame, bool flipx) const
{
    frame %= m_frames.size();

    auto offset_delta = scale * (m_frames[frame].offset - m_scaled_origin);
    auto scale_vec = glm::vec3(scale * m_aspect * m_frames[frame].scale.x, scale * m_frames[frame].scale.y, 1.0f);
    if (flipx) { // TODO This implementation of flipx only works when the sprite origin has x=0.5, which is currently true for the only sprite ¯\_(ツ)_/¯
        scale_vec.x *= -1.0f;
        offset_delta.x *= -1.0f;
    }

    const auto rotated_delta = glm::rotate(offset_delta, rotation_rads);
    const auto m =
        glm::translate(position + glm::vec3(rotated_delta, 0.0f)) *
        glm::rotate(rotation_rads, glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(scale_vec) *
        glm::mat4x4(1.0f);

    glUniform4fv(glGetUniformLocation(m_program, "sprite_source"), 1, glm::value_ptr(m_frames[frame].sprite_source));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteRenderer::done() const
{
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}