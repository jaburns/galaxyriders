#include "level.hpp"

#include "geometry.hpp"
#include <iostream>

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

Level Level::from_data(std::vector<int32_t> data)
{
    Level level;
    size_t read = 0;

    auto total_polys = data[read++];
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
