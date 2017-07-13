#include "level.hpp"

#include "geometry.hpp"
#include <iostream>

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
            this_handle.point.x = fixed32::from_raw_int(data[read++]);
            this_handle.point.y = fixed32::from_raw_int(data[read++]);
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
                const fixed32 iter = fixed32::from_fraction(1, MAX(poly.handles[i+1].quality, poly.handles[i+2].quality));
                for(fixed32 t = iter; t < fixed32::ONE; t += iter) {
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

static BakedLevel::CollisionResult test_circle(const BakedLevel& level, const fixed32::vec2 pos, fixed32 r)
{
    const auto r2 = r * r;
    bool lined = false;
    fixed32::vec2 normal, test_pos;

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
                    normal = projection / p2.sqrt();
                    if (fixed32::cross(projection, b - a) > 0) {
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
                normal = ds / d2.sqrt();
                test_pos = poly.points[i] + normal * r;
                return { true, normal, test_pos };
            }
        }
    }

    return { false };
}

BakedLevel::CollisionResult BakedLevel::collide_circle(fixed32::vec2 from, fixed32::vec2 to, fixed32 radius) const
{
    const auto r2 = radius * radius;
    auto ds = to - from;
    auto d2 = ds.x*ds.x + ds.y*ds.y;

    const auto first_check = test_circle(*this, from, r2);
    if (first_check.collided) return first_check;
    if (d2 < r2) return first_check;

    const auto dist = d2.sqrt();
    const fixed32::vec2 step = radius * (ds / dist);

    do {
        from += step;
        const auto check = test_circle(*this, from, r2);
        if (check.collided) return check;

        ds = to - from;
        d2 = ds.x*ds.x + ds.y*ds.y;
    }
    while (d2 >= r2);

    return test_circle(*this, to, r2);
}
