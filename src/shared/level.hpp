#pragma once

#include <vector>
#include <cstdint>
#include "fixed32.hpp"

struct Level
{
    struct Handle {
        fixed32::vec2 point;
        uint32_t quality;
    };

    struct Poly {
        std::vector<Handle> handles;
    };

    std::vector<Poly> polys;

    static Level from_data(std::vector<int32_t> data);
};

struct BakedLevel
{
    struct Poly {
        std::vector<fixed32::vec2> points;
    };

    std::vector<Poly> polys;

    static BakedLevel from_level(const Level& level);
};
