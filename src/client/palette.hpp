#pragma once
#include "../shared/global.hpp"

#include <glm/vec3.hpp>

static glm::vec3 from_hex(unsigned int color)
{
    return { (float)(color >> 16) / 255.0f, (float)((color >> 8) & 0xFF) / 255.0f, (float)(color & 0xFF) / 255.0f };
}

namespace Palette
{
    static const glm::vec3 COLOR_SPACE = from_hex(0x000000);
    static const glm::vec3 COLOR_DIRT  = { 0.054901f, 0.372549f, 0.462745f };
    static const glm::vec3 COLOR_TURF  = { 0.031372f, 0.239215f, 0.337254f };
    static const glm::vec3 COLOR_LIFE  = from_hex(0xFFFFFF); // { 0.843137f, 0.874509f, 0.443137f };
}
