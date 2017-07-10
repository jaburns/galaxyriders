#include "level.hpp"

void Level::bake(BakedLevel& result)
{
    result.polys.clear();
    result.polys.resize(1);

    BakedLevel::Poly& p = result.polys[0];
    p.points.push_back({ fixed32::ZERO, fixed32::ZERO });
    p.points.push_back({ fixed32::ZERO, fixed32::ONE });
    p.points.push_back({ fixed32::ONE, fixed32::ZERO });
}
