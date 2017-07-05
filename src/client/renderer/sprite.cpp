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
    bool first_iter = true;

    auto *atlas = doc.FirstChildElement();
    for (auto *child = atlas->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
        float sprite_x = child->FloatAttribute("x");
        float sprite_y = child->FloatAttribute("y");
        float sprite_width = child->FloatAttribute("width");
        float sprite_height = child->FloatAttribute("height");
        float frame_offset_x = child->FloatAttribute("frameX");
        float frame_offset_y = child->FloatAttribute("frameY");
        float frame_width = child->FloatAttribute("frameWidth");
        float frame_height = child->FloatAttribute("frameHeight");

        new_frame.sprite_source.x = sprite_x / SPRITE_SHEET_SIZE;
        new_frame.sprite_source.y = sprite_y / SPRITE_SHEET_SIZE; 
        new_frame.sprite_source.z = new_frame.sprite_source.x + sprite_width / SPRITE_SHEET_SIZE;
        new_frame.sprite_source.w = new_frame.sprite_source.y + sprite_height / SPRITE_SHEET_SIZE;

        new_frame.scale.x = sprite_width / frame_width;
        new_frame.scale.y = sprite_height / frame_height;

        new_frame.offset.x = -frame_offset_x / frame_height; // Sprite frame is one world unit tall.
        new_frame.offset.y = -(frame_offset_y + sprite_height) / frame_height;

        _frames.push_back(new_frame);

        if (first_iter) {
            _aspect = frame_width / frame_height;
            _origin.x = sprite_width / frame_height / 2.0f;
            _origin.y = -sprite_height / frame_height;
            first_iter = false;
        }
    }
}

SpriteRenderer::~SpriteRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vertex_buffer);
}

const int slowness = 5;

void SpriteRenderer::use(const glm::mat4x4& view, const glm::mat4x4& projection)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *_texture);

    glUseProgram(*_program);
    glUniformMatrix4fv(glGetUniformLocation(*_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(*_program, "sprite_texture"), 0);

    glBindVertexArray(_vao);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SpriteRenderer::draw(const glm::vec3& position, int frame)
{
    frame %= _frames.size() * slowness;
    frame /= slowness;

    glm::vec3 new_pos = position + glm::vec3(_frames[frame].offset, 0.0f) - glm::vec3(_origin, 0.0f);

    auto m = glm::scale(glm::translate(glm::mat4(1.0f), new_pos), { _aspect * _frames[frame].scale.x, _frames[frame].scale.y, 1.0f });

    glUniform4fv(glGetUniformLocation(*_program, "sprite_source"), 1, glm::value_ptr(_frames[frame].sprite_source));
    glUniformMatrix4fv(glGetUniformLocation(*_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteRenderer::done()
{
    glDisable(GL_BLEND);
}