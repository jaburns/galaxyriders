#pragma once

#include "fixed32.hpp"

namespace Geometry
{
    bool lineIntersect(fixed32::vec2 p00, fixed32::vec2 p01, fixed32::vec2 p10, fixed32::vec2 p11, fixed32::vec2 *const result = nullptr);
    bool pointInLinePerpSpace(fixed32::vec2 a, fixed32::vec2 b, fixed32::vec2 p);
    fixed32::vec2 projectPointOnLine(fixed32 m, fixed32::vec2 p);
}