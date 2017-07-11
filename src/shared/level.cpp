#include "level.hpp"

BakedLevel BakedLevel::from_level(const Level& level)
{
    BakedLevel baked;

    for (auto& poly : level.polys) {
        BakedLevel::Poly this_poly;
        for (auto& handle : poly.handles) {
            this_poly.points.push_back(handle.point);
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
