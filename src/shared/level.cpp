#include "level.hpp"
#include "geometry.hpp"
#include <glm/geometric.hpp>

Level Level::from_data(const std::vector<int32_t>& data)
{
    Level level;
    size_t read = 0;

    const auto total_polys = data[read++];
    for (auto i = 0; i < total_polys; ++i) {
        Level::Poly this_poly;

        auto total_handles = data[read++];
        for (auto j = 0; j < total_handles; ++j) {
            Level::Handle this_handle;
            this_handle.quality = data[read++];
            this_handle.point.x = (float)data[read++] / 65535.0f;
            this_handle.point.y = (float)data[read++] / 65535.0f;
            this_poly.handles.push_back(this_handle);
        }

        level.polys.push_back(this_poly);
    }

    return level;
}

BakedLevel BakedLevel::from_level(const Level& level)
{
#   define MAX(x,y) ((x) > (y) ? (x) : (y))
    BakedLevel baked;

    for (auto& poly : level.polys) {
        BakedLevel::Poly this_poly;

        for (size_t i = 0; i < poly.handles.size(); ++i) {
            this_poly.points.push_back(poly.handles[i].point);

            if (i <= poly.handles.size() - 4 && poly.handles[i+1].quality > 0 && poly.handles[i+2].quality > 0) {
                const float iter = 1.0f / MAX(poly.handles[i+1].quality, poly.handles[i+2].quality);
                for(float t = iter; t < 1.0f; t += iter) {
                    this_poly.points.push_back(Geometry::evaluate_spline(
                        poly.handles[i+0].point,
                        poly.handles[i+1].point,
                        poly.handles[i+2].point,
                        poly.handles[i+3].point,
                        t
                    ));
                }
                i += 2;
            }
        }

        baked.polys.push_back(this_poly);
    }

    return baked;
#   undef MAX
}

// TODO Move these in to a vector utility file or something

static float cross2(const glm::vec2& a, const glm::vec2& b)
{
    return a.x*b.y - a.y*b.x;
}

static glm::vec2 rotate90(const glm::vec2& v)
{
    return { -v.y, v.x };
}

static glm::vec2 reflect(const glm::vec2& v, const glm::vec2& unit_normal, float normal_scale, float tangent_scale)
{
    const auto unit_tangent = rotate90(unit_normal);

    const auto norm_component = -normal_scale * glm::dot(v, unit_normal);
    const auto tang_component = tangent_scale * glm::dot(v, unit_tangent);

    return unit_normal * norm_component + unit_tangent * tang_component;
}

struct CircleTestResult {
    bool collided;
    glm::vec2 normal;
    glm::vec2 position;
};

static CircleTestResult test_circle(const BakedLevel& level, const glm::vec2 pos, float r)
{
    const auto r2 = r * r;
    bool lined = false;
    glm::vec2 normal, test_pos;

    // TODO at least do a rectangle bounds check before all the Real Math.

    for (const auto& poly : level.polys) {
        test_pos = pos;

        for (auto i = 0; i < poly.points.size(); ++i) {
            const auto a = poly.points[i];
            const auto b = poly.points[(i+1) % poly.points.size()];

            if (Geometry::point_in_line_perp_space(a, b, test_pos)) {
                const auto m  = (b.y-a.y)/(b.x-a.x);
                const auto l = test_pos - a;
                const auto point_on_line = Geometry::project_point_on_line(m, l);
                const auto projection = l - point_on_line;
                const auto p2 = projection.x*projection.x + projection.y*projection.y;

                if (p2 < r2) {
                    normal = glm::normalize(projection);
                    if (cross2(projection, b - a) > 0.0f) {
                        normal = -normal;
                    }

                    test_pos = a + point_on_line + normal * r;
                    lined = true;
                }
            }
        }

        if (lined) {
            return { true, normal, test_pos };
        }

        for (auto i = 0; i < poly.points.size(); ++i) {
            const auto ds = pos - poly.points[i];
            const auto d2 = ds.x*ds.x + ds.y*ds.y;
            if (d2 < r2) {
                normal = glm::normalize(ds);
                test_pos = poly.points[i] + normal * r;
                return { true, normal, test_pos };
            }
        }
    }

    return { false };
}

BakedLevel::CollisionResult BakedLevel::move_and_collide_circle(glm::vec2 position, glm::vec2 velocity, float radius, float bounce) const
{
    position += velocity;
    const auto test = test_circle(*this, position, radius);
    bool collided = false;

    if (test.collided) {
        collided = true;
        position = test.position;
        velocity = reflect(velocity, test.normal, bounce, 1.0f);
    }

    return { test.collided, position, velocity };
    /*
    const auto r2 = radius * radius;
    auto d2 = velocity.x*velocity.x + velocity.y*velocity.y;

    bool collided = false;
    const auto step = radius * glm::normalize(velocity);
    while (d2 >= r2) {
        position += step;
        velocity -= step;

        const auto test = test_circle(*this, position, radius);
        if (test.collided) {
            collided = true;
            position = test.position;
            velocity = reflect(velocity, test.normal, bounce, 1.0f);
        }

        d2 = velocity.x*velocity.x + velocity.y*velocity.y;
    }

    position += velocity;

    const auto test = test_circle(*this, position, radius);

    if (test.collided) {
        collided = true;
        position = test.position;
        velocity = reflect(velocity, test.normal, bounce, 1.0f);
    }

    return { collided, position, velocity };
    */
}
