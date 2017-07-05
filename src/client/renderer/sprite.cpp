#include "sprite.hpp"

#include <tinyxml2.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

static GLfloat quad_vertices[] = {
     0.0f,  1.0f, 0.0f, 
     1.0f,  0.0f, 0.0f,
     0.0f,  0.0f, 0.0f,
     1.0f,  0.0f, 0.0f, 
     0.0f,  1.0f, 0.0f, 
     1.0f,  1.0f, 0.0f
};

SpriteRenderer::SpriteRenderer()
{
    _program = std::make_unique<const ShaderProgram>("res/shaders/sprite.vert", "res/shaders/sprite.frag");;
    _texture = std::make_unique<const Texture>("res/sprites/guy_frames.png");
    this->load_frames();

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*_program, "position"), 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void SpriteRenderer::load_frames()
{
    using namespace tinyxml2;
    const float SPRITE_SHEET_SIZE = 512.0f;

    SpriteFrame new_frame;
    XMLDocument doc;
    doc.LoadFile("res/sprites/guy_frames.xml");

    auto *atlas = doc.FirstChildElement();
    for (auto *child = atlas->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        float sprite_x = child->FloatAttribute("x") / SPRITE_SHEET_SIZE;
        float sprite_y = child->FloatAttribute("y") / SPRITE_SHEET_SIZE;
        float sprite_width = child->FloatAttribute("width") / SPRITE_SHEET_SIZE;
        float sprite_height = child->FloatAttribute("height") / SPRITE_SHEET_SIZE;
        float frame_offset_x = child->FloatAttribute("frameX") / SPRITE_SHEET_SIZE;
        float frame_offset_y = child->FloatAttribute("frameY") / SPRITE_SHEET_SIZE;
        float frame_width = child->FloatAttribute("frameWidth") / SPRITE_SHEET_SIZE;
        float frame_height = child->FloatAttribute("frameHeight") / SPRITE_SHEET_SIZE;

        // Character stretches on x-axis through animation for some reason

        new_frame.sprite_source.x = sprite_x;
        new_frame.sprite_source.y = sprite_y;
        new_frame.sprite_source.z = sprite_x + sprite_width;
        new_frame.sprite_source.w = sprite_y + sprite_height;

        new_frame.sprite_frame.x = sprite_x + frame_offset_x;
        new_frame.sprite_frame.y = sprite_y + frame_offset_y;
        new_frame.sprite_frame.z = new_frame.sprite_source.x + frame_width;
        new_frame.sprite_frame.w = new_frame.sprite_source.y + frame_height;

        _frames.push_back(new_frame);

        _scale_x = frame_width / frame_height;
    }
}

SpriteRenderer::~SpriteRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vertex_buffer);
}

static int frame = 0;
const int slowness = 10;

void SpriteRenderer::use(const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *_texture);

    glUseProgram(*_program);
    glUniformMatrix4fv(glGetUniformLocation(*_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform4fv(glGetUniformLocation(*_program, "sprite_source"), 1, glm::value_ptr(_frames[frame / slowness].sprite_source));
    glUniform4fv(glGetUniformLocation(*_program, "sprite_frame"), 1, glm::value_ptr(_frames[frame / slowness].sprite_frame));
    glUniform1i(glGetUniformLocation(*_program, "sprite_texture"), 0);

    glBindVertexArray(_vao);

    frame += 1;
    frame %= _frames.size() * slowness;
}

void SpriteRenderer::draw(const glm::vec3& position)
{
    auto m = glm::scale(glm::translate(glm::mat4(1.0f), position), { _scale_x, 1.0f, 1.0f });

    glUniformMatrix4fv(glGetUniformLocation(*_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);
}