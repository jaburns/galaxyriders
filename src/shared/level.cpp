#include "level.hpp"
#include "geometry.hpp"
#include <glm/geometric.hpp>

Level Level::from_data(const std::vector<float>& data)
{
    Level level;
    size_t read = 0;

    const auto total_polys = data[read++];
    for (int i = 0; i < total_polys; ++i) {
        Level::Poly this_poly;

        int total_handles = data[read++];
        for (int j = 0; j < total_handles; ++j) {
            Level::Handle this_handle;
            this_handle.quality = (uint32_t)data[read++];
            this_handle.point.x = data[read++];
            this_handle.point.y = data[read++];
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

        for (auto i = 0; i < poly.handles.size(); ++i) {
            this_poly.points.push_back(poly.handles[i].point);

            if (i <= poly.handles.size() - 4 && poly.handles[i+1].quality > 0 && poly.handles[i+2].quality > 0) {
                const auto iter = 1.0f / MAX(poly.handles[i+1].quality, poly.handles[i+2].quality);
                for(auto t = iter; t < 1.0f; t += iter) {
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
                    if (Geometry::vec2_cross(projection, b - a) > 0.0f) {
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
    glm::vec2 normal(0.0f);
    bool collided = false;
    auto distance_remaining = glm::length(velocity);

    while (distance_remaining > radius) {
        const auto step = glm::normalize(velocity) * radius;

        position += step;
        distance_remaining -= radius;

        const auto test = test_circle(*this, position, radius);
        if (test.collided) {
            collided = true;
            position = test.position;
            velocity = Geometry::vec2_reflect(velocity, test.normal, bounce, 1.0f);
            normal = test.normal;
        }
    }

    position += glm::normalize(velocity) * distance_remaining;

    const auto test = test_circle(*this, position, radius);

    if (test.collided) {
        collided = true;
        position = test.position;
        velocity = Geometry::vec2_reflect(velocity, test.normal, bounce, 1.0f);
        normal = test.normal;
    }

    return { collided, position, velocity, normal };
}

static Level s_loaded_level = Level::from_data({1.0f,39.0f,0.0f,383.5131f,-106.7458f,0.0f,-113.6776f,-105.6847f,0.0f,-113.6746f,60.29282f,0.0f,-96.52872f,32.61086f,0.0f,-50.92356f,45.47921f,0.0f,-18.6964f,45.29268f,50.0f,-25.01754f,-11.05426f,50.0f,-17.72211f,-17.35335f,0.0f,-7.363365f,-17.46777f,0.0f,42.68919f,-17.48897f,0.0f,42.70433f,-35.73095f,0.0f,51.56719f,-35.63853f,0.0f,51.57977f,-17.30403f,0.0f,72.65042f,-17.24803f,50.0f,84.98267f,-17.37347f,50.0f,104.8119f,-23.64916f,0.0f,119.17f,-24.08519f,0.0f,140.9189f,-24.10562f,0.0f,140.8984f,-32.22205f,0.0f,158.9607f,-32.22999f,0.0f,158.9369f,-24.02389f,0.0f,169.9889f,-24.09263f,0.0f,169.8818f,-32.26975f,0.0f,187.3026f,-32.27576f,0.0f,187.3126f,-24.13411f,0.0f,198.1484f,-24.13497f,0.0f,198.1618f,-32.24654f,0.0f,214.8907f,-32.1848f,0.0f,214.8486f,-24.13108f,0.0f,249.9169f,-24.10995f,50.0f,260.3928f,-24.31205f,50.0f,268.3945f,-22.60493f,0.0f,280.6699f,-6.108091f,0.0f,290.0646f,8.905025f,0.0f,306.7936f,9.556507f,50.0f,318.8844f,-10.8901f,50.0f,336.2576f,-6.70936f,0.0f,342.5138f,24.31914f,0.0f,383.6403f,245.7591f});
static BakedLevel s_loaded_level_baked = BakedLevel::from_level(s_loaded_level);

Level& LoadedLevel::get()
{
    return s_loaded_level;
}

void LoadedLevel::set(const Level& level)
{
    s_loaded_level = level;
    bake();
}

void LoadedLevel::bake()
{
    s_loaded_level_baked = BakedLevel::from_level(s_loaded_level);
}

const BakedLevel& LoadedLevel::get_baked()
{
    return s_loaded_level_baked;
}
