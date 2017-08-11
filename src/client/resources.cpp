#include "resources.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <lodepng.h>

static GLuint shader_compile_from_file(const std::string& shader_path, GLenum shader_type)
{
    std::ifstream file(shader_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string str = buffer.str();
    const char *cstr = str.c_str();

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &cstr, NULL);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<char> log(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, log.data());
        std::cout << "Error in shader: " << shader_path << std::endl;
        std::cout << log.data() << std::endl;
        exit(EXIT_FAILURE);
    }

    return shader;
}

ShaderProgram::ShaderProgram(const std::string& vert_path, const std::string& frag_path)
{
    m_id = glCreateProgram();
    GLuint vert = shader_compile_from_file(vert_path, GL_VERTEX_SHADER);
    GLuint frag = shader_compile_from_file(frag_path, GL_FRAGMENT_SHADER);
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);
    glDetachShader(m_id, vert);
    glDetachShader(m_id, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
}

ShaderProgram::~ShaderProgram()
{
    if (m_id != 0) {
        glDeleteProgram(m_id);
    }
}

Texture::Texture(const std::string& png_path)
{
    std::vector<uint8_t> image;
    uint32_t width, height;
    uint32_t error = lodepng::decode(image, width, height, png_path);

    if (error != 0) {
        std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
        return;
    }

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
    }
}

CubeMap::CubeMap(const std::string& r, const std::string& l, const std::string& t, const std::string& bo, const std::string& ba, const std::string& f)
{
    glGenTextures(1, &m_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    const std::string* sides[6] = { &r, &l, &t, &bo, &ba, &f };

    for (int i = 0; i < 6; ++i) {
        std::vector<uint8_t> image;
        uint32_t width, height;
        uint32_t error = lodepng::decode(image, width, height, *sides[i]);
        if (error != 0) {
            std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
            return;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap()
{
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
    }
}