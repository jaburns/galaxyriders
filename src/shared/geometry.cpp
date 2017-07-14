#include "geometry.hpp"

#include <vector>

bool Geometry::line_intersect(glm::vec2 p00, glm::vec2 p01, glm::vec2 p10, glm::vec2 p11, glm::vec2 *const result)
{
    auto dx1x3 = p00.x - p10.x;
    auto dy1y3 = p00.y - p10.y;
    auto dx2x1 = p01.x - p00.x;
    auto dy2y1 = p01.y - p00.y;
    auto dx4x3 = p11.x - p10.x;
    auto dy4y3 = p11.y - p10.y;

    auto denom = dy4y3*dx2x1 - dx4x3*dy2y1;
    auto numa  = dx4x3*dy1y3 - dy4y3*dx1x3;
    auto numb  = dx2x1*dy1y3 - dy2y1*dx1x3;

    if (denom == 0) return false;

    numa /= denom;
    numb /= denom;

    if (numa >= 0 && numa <= 1 && numb >= 0 && numb <= 1) {
        if (result != nullptr) {
            result->x = p00.x + dx2x1*numa;
            result->y = p00.y + dy2y1*numa;
        }
        return true;
    }

    return false;
}

bool Geometry::point_in_line_perp_space(glm::vec2 a, glm::vec2 b, glm::vec2 p)
{
    float _ax, _ay, _bx, _by, _cx, _cy;
    float perpSlope = (a.x-b.x) / (b.y-a.y);

    // If the slope is greater than 1, transpose the coordinate space to avoid infinity.
    if (perpSlope > 1) {
        _ax = a.y; _bx = b.y; _cx = p.y;
        _ay = a.x; _by = b.x; _cy = p.x;
        perpSlope = (_ax-_bx)/(_by-_ay);
    } else {
        _ax = a.x; _bx = b.x; _cx = p.x;
        _ay = a.y; _by = b.y; _cy = p.y;
    }

    float yMin, yMax;

    if (_ay > _by) {
        yMin = perpSlope*(_cx - _bx) + _by;
        yMax = perpSlope*(_cx - _ax) + _ay;
    } else {
        yMin = perpSlope*(_cx - _ax) + _ay;
        yMax = perpSlope*(_cx - _bx) + _by;
    }

    return _cy > yMin && _cy < yMax;
}

static const float ACCURACY     = float(10000);
static const float ACCURACY_INV = 1.0f / ACCURACY;

glm::vec2 Geometry::project_point_on_line(float m, glm::vec2 p)
{
    //TODO find an abs somewhere
#   define ABS(x) ((x) < 0.0f ? -(x) : (x))
    auto abs_m = ABS(m);
    if (abs_m < ACCURACY_INV) {
        return { p.x, 0.0f };
    } else if (abs_m > ACCURACY) {
        return { 0.0f, p.y };
    }

    glm::vec2 result;
    result.y = m*(m*p.y + p.x) / (1.0f + m*m);
    result.x = result.y / m;
    return result;
}

static const float THREE = float(3);

glm::vec2 Geometry::evaluate_spline(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d, float t)
{
    auto u = 1.0f - t;
    auto u2 = u * u;
    auto t2 = t * t;
    return u2*u*a + THREE*u2*t*b + THREE*u*t2*c + t2*t*d;
}
