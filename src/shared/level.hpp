#pragma once

#include <vector>
#include <cstdint>
#include "fixed32.hpp"

struct BakedLevel
{
    struct Poly {
        std::vector<fixed32::vec2> points;
    };

    std::vector<Poly> polys;
};

struct Level
{
    struct Handle {
        fixed32::vec2 point;
        uint8_t quality;
    };

    struct Poly {
        std::vector<Handle> handles;
    };

    std::vector<Poly> polys;

    void bake(BakedLevel& result);
};
