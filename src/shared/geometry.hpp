#pragma once

#include <vector>
#include <glm/vec2.hpp>

namespace Geometry
{
    bool line_intersect(glm::vec2 p00, glm::vec2 p01, glm::vec2 p10, glm::vec2 p11, glm::vec2 *const result = nullptr);
    bool point_in_line_perp_space(glm::vec2 a, glm::vec2 b, glm::vec2 p);
    glm::vec2 project_point_on_line(float m, glm::vec2 p);
    glm::vec2 evaluate_spline(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d, float t);
}
