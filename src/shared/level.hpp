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

    static Level from_data(const std::vector<float>& data);
};

struct BakedLevel
{
    struct Poly {
        std::vector<glm::vec2> points;
    };

    struct CollisionResult {
        bool collided = false;
        glm::vec2 position;
        glm::vec2 velocity;
    };

    std::vector<Poly> polys;

    static BakedLevel from_level(const Level& level);

    CollisionResult move_and_collide_circle(glm::vec2 position, glm::vec2 delta, float radius, float bounce) const;
};
