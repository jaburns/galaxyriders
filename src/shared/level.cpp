#include "level.hpp"

#include "geometry.hpp"
#include <iomanip>
#include <sstream>
#include <glm/geometric.hpp>

#define MAX(x,y) ((x) > (y) ? (x) : (y))

static std::string floats_to_string(const std::vector<float>& floats)
{
    std::stringstream result;
    result << std::hex << std::setfill('0');

    for (int i = 0; i < floats.size(); ++i)
        result << std::setw(8) << *reinterpret_cast<const uint32_t*>(&floats[i]);

    return result.str();
}

static std::vector<float> string_to_floats(const std::string &str)
{
    std::vector<float> result;

    for (int i = 0, len = str.size(); i < len; i += 8)
    {
        std::istringstream converter(str.substr(i, 8));
        uint32_t int_value;
        converter >> std::hex >> int_value;
        result.push_back(*reinterpret_cast<const float*>(&int_value));
    }
    
    return result;
}

Level::Level(const std::vector<float>& data)
{
    size_t read = 0;
    curve_quality = data[read++];
    const float total_polys = data[read++];

    for (int i = 0; i < total_polys; ++i) 
    {
        Level::Poly this_poly;
        int total_handles = data[read++];

        for (int j = 0; j < total_handles; ++j) 
        {
            Level::Handle this_handle;
            this_handle.is_curve = (uint32_t)data[read++] > 0;
            this_handle.point.x = data[read++];
            this_handle.point.y = data[read++];
            this_poly.handles.push_back(this_handle);
        }

        polys.push_back(this_poly);
    }
}

Level::Level(const std::string& serialized_level)
    : Level(string_to_floats(serialized_level))
{ }

uint32_t Level::checksum() const
{
    uint32_t sum = 0;

    for (const auto& p : polys)
    for (const auto& h : p.handles) 
    {
        sum += *reinterpret_cast<const uint32_t*>(&h.point.x);
        sum += *reinterpret_cast<const uint32_t*>(&h.point.y);
        sum *= h.is_curve ? 1 : 2;
    }

    sum ^= *reinterpret_cast<const uint32_t*>(&curve_quality);

    return sum;
}

std::string Level::serialize() const
{
    std::vector<float> floats;

    floats.push_back(curve_quality);
    floats.push_back(polys.size());

    for (const auto& p : polys)
    {
        floats.push_back(p.handles.size());

        for (const auto& h : p.handles)
        {
            floats.push_back(h.is_curve ? 1.0f : 0.0f);
            floats.push_back(h.point.x);
            floats.push_back(h.point.y);
        }
    }

    return floats_to_string(floats);
}

static float get_dynamic_curve_quality(float quality, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d)
{
    const float ESTIMATION_STEP = 0.1f;

    float length_estimate = 0.0f;
    glm::vec2 last_pt = a;

    for (float t = ESTIMATION_STEP; t < 1.0f; t += ESTIMATION_STEP)
    {
        glm::vec2 next_pt = Geometry::evaluate_spline(a, b, c, d, t);
        length_estimate += glm::length(next_pt - last_pt);
        last_pt = next_pt;
    }

    return quality * length_estimate;
}

BakedLevel::BakedLevel(const Level& level)
{
    for (auto& poly : level.polys) 
    {
        BakedLevel::Poly this_poly;

        for (auto i = 0; i < poly.handles.size(); ++i) 
        {
            this_poly.points.push_back({ poly.handles[i].point, false });

            if (i <= poly.handles.size() - 4 && poly.handles[i+1].is_curve && poly.handles[i+2].is_curve) 
            {
                const auto a = poly.handles[i+0].point;
                const auto b = poly.handles[i+1].point;
                const auto c = poly.handles[i+2].point;
                const auto d = poly.handles[i+3].point;

                const float iter = 1.0f / get_dynamic_curve_quality(level.curve_quality, a, b, c, d);

                for(auto t = iter; t < 1.0f; t += iter) 
                {
                    this_poly.points.push_back({
                        Geometry::evaluate_spline(a, b, c, d, t), 
                        true
                    });
                }

                i += 2;
            }
        }

        polys.push_back(this_poly);
    }
}

struct CircleTestResult 
{
    bool collided;
    std::vector<glm::vec2> normals;
    glm::vec2 position;
};

static CircleTestResult test_circle(const BakedLevel& level, const glm::vec2 pos, float r)
{
    const auto r2 = r * r;
    std::vector<glm::vec2> normals;
    glm::vec2 test_pos = pos;

    // TODO at least do a rectangle bounds check before all the Real Math.

    for (const auto& poly : level.polys) {
        for (auto i = 0; i < poly.points.size(); ++i) {
            const auto a = poly.points[i].pos;
            const auto b = poly.points[(i + 1) % poly.points.size()].pos;

            if (Geometry::point_in_line_perp_space(a, b, test_pos)) {
                const auto m = (b.y - a.y) / (b.x - a.x);
                const auto l = test_pos - a;
                const auto point_on_line = Geometry::project_point_on_line(m, l);
                const auto projection = l - point_on_line;
                const auto p2 = projection.x*projection.x + projection.y*projection.y;

                if (p2 < r2) {
                    auto normal = glm::normalize(projection);
                    if (Geometry::vec2_cross(projection, b - a) > 0.0f) {
                        normal = -normal;
                    }
                    test_pos = a + point_on_line + normal * r;
                    normals.push_back(normal);
                }
            }
        }

        if (normals.size() > 0) {
            return { true, normals, test_pos };
        }

        for (auto i = 0; i < poly.points.size(); ++i) {
            const auto ds = pos - poly.points[i].pos;
            const auto d2 = ds.x*ds.x + ds.y*ds.y;
            if (d2 < r2) {
                const auto normal = glm::normalize(ds);
                test_pos = poly.points[i].pos + normal * r;
                normals.push_back(normal);
                return { true, normals, test_pos };
            }
        }
    }

    return { false };
}

static glm::vec2 upmost(const std::vector<glm::vec2>& vecs)
{
    float most_y = -1e9f;
    glm::vec2 result;

    for (const auto& v : vecs) {
        if (v.y > most_y) {
            result = v;
            most_y = v.y;
        }
    }

    return result;
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

            for (const auto& normal : test.normals) {
                velocity = Geometry::vec2_reflect(velocity, normal, bounce, 1.0f);
            }

            normal = upmost(test.normals);
        }
    }

    position += glm::normalize(velocity) * distance_remaining;

    const auto test = test_circle(*this, position, radius);
    if (test.collided) {
        collided = true;
        position = test.position;

        for (const auto& normal : test.normals) {
            velocity = Geometry::vec2_reflect(velocity, normal, bounce, 1.0f);
        }

        normal = upmost(test.normals);
    }

    return { collided, position, velocity, normal };
}

// TODO refactor staticly loaded baked level in to real code
static Level s_loaded_level("3f4000003f800000421c00000000000043bfc1adc2d57dd900000000c2e35aeec2d35e9100000000c2e3596542712bd900000000c2c10eb44202718500000000c24bb1ba4235eab600000000c195923a42352bb43f800000c1c823ecc130de403f800000c18dc6e2c18ad3a900000000c0eba0b0c18bbdfe00000000422ac1bbc18be96900000000422ad13cc20eec7e00000000424e44cdc20e8ddb00000000424e51afc18a6ea70000000042914d04c189fbf73f80000042a9f721c18afcde3f80000042d19fb1c1bd317b0000000042ee570ac1c0ae7800000000430ceb3dc1c0d84f00000000430ce5fec200e36100000000431ef5f0c200eb8200000000431eefd9c1c030ed000000004329fd29c1c0bdb5000000004329e1bec201143900000000433b4d77c2011a6100000000433b5007c1c112a800000000434625fec1c1146b000000004346296cc200fc75000000004356e405c200bd3c000000004356d93ec1c10c74000000004379eabac1c0e12d3f80000043823247c1c27f143f8000004386327fc1b4d6e600000000438c55bfc0c3757b0000000043910845410e7afb00000000439965954118e7743f800000439f7134c12e3dd93f80000043a820f9c0d6b3140000000043ab41c441c28d990000000043bfd1f54375c254");
static BakedLevel s_loaded_level_baked(s_loaded_level);

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
    s_loaded_level_baked = BakedLevel(s_loaded_level);
}

const BakedLevel& LoadedLevel::get_baked()
{
    return s_loaded_level_baked;
}
