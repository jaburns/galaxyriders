#pragma once

#include <vector>
#include <cstdint>
#include <glm/vec2.hpp>

struct Level
{
    struct Handle {
        glm::vec2 point;
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
        std::vector<glm::vec2> points;
    };

    struct CollisionResult {
        bool collided = false;
        glm::vec2 normal;
        glm::vec2 restore;
    };

    std::vector<Poly> polys;

    static BakedLevel from_level(const Level& level);

    CollisionResult collide_circle(glm::vec2 from, glm::vec2 to, float radius) const;
};
