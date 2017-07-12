#pragma once

#include <vector>
#include "fixed32.hpp"

namespace Geometry
{
    bool line_intersect(fixed32::vec2 p00, fixed32::vec2 p01, fixed32::vec2 p10, fixed32::vec2 p11, fixed32::vec2 *const result = nullptr);
    bool point_in_line_perp_space(fixed32::vec2 a, fixed32::vec2 b, fixed32::vec2 p);
    fixed32::vec2 project_point_on_line(fixed32 m, fixed32::vec2 p);
    fixed32::vec2 evaluate_bezier(const std::vector<fixed32::vec2>& points, fixed32 t);
}