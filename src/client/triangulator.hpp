#include <vector>
#include <cstdint>
#include "../shared/fixed32.hpp"

namespace Triangulator
{
    std::vector<uint32_t> triangulate(const std::vector<fixed32::vec2>& pts);
}
