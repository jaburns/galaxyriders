#include "level.hpp"

#include "geometry.hpp"
#include <iostream>

BakedLevel BakedLevel::from_level(const Level& level)
{
    const fixed32 iter = fixed32::from_fraction(1, 20);
    BakedLevel baked;

    for (auto& poly : level.polys) {
        BakedLevel::Poly this_poly;

        for (size_t i = 0; i < poly.handles.size(); ) {

            std::vector<fixed32::vec2> current_curve;

            current_curve.push_back(poly.handles[i++].point);
            while (i < poly.handles.size() && poly.handles[i].quality > 0) {
                current_curve.push_back(poly.handles[i++].point);
            }

            if (current_curve.size() >= 2) {
                if (i < poly.handles.size()) {
                    current_curve.push_back(poly.handles[i].point);
                } else {
                    current_curve.push_back(poly.handles[0].point);
                }
                for(fixed32 t = fixed32::ZERO; t < fixed32::ONE; t += iter) {
                    this_poly.points.push_back(Geometry::evaluate_bezier(current_curve, t));
                }
            } else {
                this_poly.points.push_back(current_curve[0]);
            }
        }

        baked.polys.push_back(this_poly);
    }

    return baked;
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
