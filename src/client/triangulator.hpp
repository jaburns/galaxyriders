#pragma once
#include "../shared/global.hpp"

#include <vector>
#include <cstdint>
#include <glm/vec2.hpp>

namespace Triangulator
{
    std::vector<uint32_t> triangulate(const std::vector<glm::vec2>& pts);
}
