#pragma once

#include "../glfw.hpp"
#include "../resources.hpp"
#include "../../shared/level.hpp"

class LevelRenderer
{
    GLuint m_vao;
    GLuint m_vertex_buffer;

    LevelRenderer(const LevelRenderer&) =delete;
    LevelRenderer& operator=(const LevelRenderer&) =delete;

public:
    LevelRenderer(const BakedLevel& baked_level);
    ~LevelRenderer();

    void draw_once();
};
