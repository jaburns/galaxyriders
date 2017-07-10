#include "geometry.hpp"

bool Geometry::lineIntersect(fixed32::vec2 p00, fixed32::vec2 p01, fixed32::vec2 p10, fixed32::vec2 p11, fixed32::vec2 *const result)
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

bool Geometry::pointInLinePerpSpace(fixed32::vec2 a, fixed32::vec2 b, fixed32::vec2 p) 
{
    fixed32 _ax, _ay, _bx, _by, _cx, _cy;
    fixed32 perpSlope = (a.x-b.x) / (b.y-a.y);
    
    // If the slope is greater than 1, transpose the coordinate space to avoid infinity.
    if (perpSlope > 1) {
        _ax = a.y; _bx = b.y; _cx = p.y;
        _ay = a.x; _by = b.x; _cy = p.x;
        perpSlope = (_ax-_bx)/(_by-_ay);
    } else {
        _ax = a.x; _bx = b.x; _cx = p.x;
        _ay = a.y; _by = b.y; _cy = p.y;
    }
    
    fixed32 yMin, yMax;
    
    if (_ay > _by) {
        yMin = perpSlope*(_cx - _bx) + _by;
        yMax = perpSlope*(_cx - _ax) + _ay;
    } else {
        yMin = perpSlope*(_cx - _ax) + _ay;
        yMax = perpSlope*(_cx - _bx) + _by;
    }
    
    return _cy > yMin && _cy < yMax;
}

static const fixed32 ACCURACY     = fixed32(10000);
static const fixed32 ACCURACY_INV = fixed32::ONE / ACCURACY;

fixed32::vec2 Geometry::projectPointOnLine(fixed32 m, fixed32::vec2 p)
{
    auto abs_m = m.abs();
    if (abs_m < ACCURACY_INV) {
        return { p.x, fixed32::ZERO };
    }
    else if (abs_m > ACCURACY) {
        return { fixed32::ZERO, p.y };
    }

    fixed32::vec2 result;
    result.y = m*(m*p.y + p.x) / (fixed32::ONE + m*m);
    result.x = result.y / m;
    return result;
}