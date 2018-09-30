#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <glm/vec2.hpp>

struct Level
{
    struct Handle 
    {
        glm::vec2 point;
        bool is_curve;
    };

    struct Poly 
    {
        std::vector<Handle> handles;
    };

    float curve_quality = 1.0f;
    std::vector<Poly> polys;

    Level() {}
    Level(const std::vector<float>& data);
    Level(const std::string& serialized_level);

    uint32_t checksum() const;
    std::string serialize() const;
};

struct BakedLevel
{
    struct Point 
    {
        glm::vec2 pos;
        bool is_curve = false;
    };

    struct Poly 
    {
        std::vector<Point> points;
    };

    struct CollisionResult 
    {
        bool collided = false;
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 normal;
    };

    std::vector<Poly> polys;

    BakedLevel() {}
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