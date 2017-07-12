#include "level.hpp"

#include "geometry.hpp"
#include <iostream>

/*

383.5131::-106.7458::0~~-113.6776::-105.6847::0~~-113.6746::60.29282::0~~-96.52872::32.61086::0~~-50.92356::45.47921::0~~-18.6964::45.29268::0~~-25.01754::-11.05426::10~~-17.72211::-17.35335::10~~-7.363365::-17.46777::0~~42.68919::-17.48897::0~~42.70433::-35.73095::0~~51.56719::-35.63853::0~~51.57977::-17.30403::0~~72.65042::-17.24803::0~~84.98267::-17.37347::10~~104.8119::-23.64916::10~~119.17::-24.08519::0~~140.9189::-24.10562::0~~140.8984::-32.22205::0~~158.9607::-32.22999::0~~158.9369::-24.02389::0~~169.9889::-24.09263::0~~169.8818::-32.26975::0~~187.3026::-32.27576::0~~187.3126::-24.13411::0~~198.1484::-24.13497::0~~198.1618::-32.24654::0~~214.8907::-32.1848::0~~214.8486::-24.13108::0~~249.9169::-24.10995::0~~260.3928::-24.31205::10~~268.3945::-22.60493::10~~279.9323::-5.717587::0~~290.0646::8.905025::0~~306.7936::9.556507::0~~318.8844::-10.8901::10~~336.2576::-6.70936::19~~342.5138::24.31914::0~~383.6403::245.7591::0~~

    List<Vector2> generatePoints()
    {
        var points = new List<Vector2>();

        for (int i = 0; i < targ.Points.Count; ) {
            if (i <= targ.Points.Count - 4 && targ.Points[i+1].IsCurveHandle && targ.Points[i+2].IsCurveHandle) {
                points.Add(targ.Points[i+0].Position);
                for (int j = 1; j < targ.Points[i+0].Quality; ++j) {
                    points.Add(spline(
                        targ.Points[i+0].Position,
                        targ.Points[i+1].Position,
                        targ.Points[i+2].Position,
                        targ.Points[i+3].Position,
                        (float)j / targ.Points[i+0].Quality
                    ));
                }

                if (i + 3 == targ.Points.Count - 1) {
                    points.Add(targ.Points[i+3].Position);
                    break;
                }
                i += 3;
            }
            else {
                points.Add(targ.Points[i].Position);
                i += 1;
            }
        }

        return points;
    }
*/

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
