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

    static Level from_data(const std::vector<int32_t>& data);
};

struct BakedLevel
{
    struct Poly {
        std::vector<fixed32::vec2> points;
    };

    struct CollisionResult {
        bool collided = false;
        fixed32::vec2 normal;
        fixed32::vec2 restore;
    };

    std::vector<Poly> polys;

    static BakedLevel from_level(const Level& level);

    CollisionResult collide_circle(fixed32::vec2 from, fixed32::vec2 to, fixed32 radius) const;
};
