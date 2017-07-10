#pragma once

#include <vector>
#include "fixed32.hpp"

struct Level
{
    struct Poly {
        std::vector<fixed32::vec2> points;
    };

    std::vector<Poly> polys;
};

struct BakedLevel
{
    struct Line {
        fixed32::vec2 a, b;
    };

    std::vector<Line> lines;
};