#include "resources.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <lodepng.h>

static GLuint shader_compile_from_file(const char *shader_path, GLenum shader_type)
{
    std::ifstream file(shader_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string str = buffer.str();
    const char *cstr = str.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);

    return shader;
}

ShaderProgram::ShaderProgram(const char *vert_path, const char *frag_path)
{
    _id = glCreateProgram();
    GLuint vert = shader_compile_from_file(vert_path, GL_VERTEX_SHADER);
    GLuint frag = shader_compile_from_file(frag_path, GL_FRAGMENT_SHADER);
    glAttachShader(_id, vert);
    glAttachShader(_id, frag);
    glLinkProgram(_id);
    glDetachShader(_id, vert);
    glDetachShader(_id, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

ShaderProgram::~ShaderProgram()
{
    if (_id != 0) {
        glDeleteProgram(_id);
    }
}


Texture::Texture(const char *png_path)
{
    std::vector<unsigned char> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, png_path);

    if (error != 0) {
        std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
        return;
    }

    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

Texture::~Texture()
{
    if (_id != 0) {
        glDeleteTextures(1, &_id);
    }
}
