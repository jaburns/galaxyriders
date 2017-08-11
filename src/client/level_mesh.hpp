#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include "../shared/level.hpp"
#include "../shared/lang_utils.hpp"

struct LevelMesh
{
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> surface_pos;
    std::vector<uint32_t> indices;

public:
    LevelMesh() {};
    LevelMesh(const BakedLevel& level);
};