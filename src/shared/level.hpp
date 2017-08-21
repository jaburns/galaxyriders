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

    Level(const std::vector<float>& data);

    uint32_t checksum();
};

struct BakedLevel
{
    struct Point {
        glm::vec2 pos;
        bool is_curve = false;
    };

    struct Poly {
        std::vector<Point> points;
    };

    struct CollisionResult {
        bool collided = false;
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 normal;
    };

    std::vector<Poly> polys;

    BakedLevel(const Level& level);

    CollisionResult move_and_collide_circle(glm::vec2 position, glm::vec2 delta, float radius, float bounce) const;
};

namespace LoadedLevel
{
    Level& get();
    void set(const Level& level);
    void bake();
    const BakedLevel& get_baked();
}
