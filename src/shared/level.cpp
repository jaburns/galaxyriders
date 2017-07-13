#include "level.hpp"

#include "geometry.hpp"
#include <iostream>

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

BakedLevel::CollisionResult BakedLevel::collide_circle() const
{
    return { true };
}

/*
 Collision.prototype._circleCollision = function (x,y,r) {
    var lines = this._lines;
    var r2 = r*r;
    var lined = false;
    var normal = null;
    var restore = null;

    for (var i = 0; i < lines.length - 1; i++) {
      var ax = lines[i].x;
      var ay = lines[i].y;
      var bx = lines[i+1].x;
      var by = lines[i+1].y;

      if (geom.pointInLinePerpSpace (ax, ay, bx, by, x, y)) {
        var m  = (by-ay)/(bx-ax);
        var lx = x-ax;
        var ly = y-ay;

        var pointOnLine = geom.projectPointOnLine (m, lx, ly);
        var projection = pointOnLine.clone().scale(-1);

        projection.x += lx;
        projection.y += ly;

        if (projection.magnitude2() < r2) {
          normal = projection.clone().normalize();
          if (projection.cross (new Vec2 (bx - ax, by - ay)) > 0) {
            normal.scale(-1);
          }
          restore = (new Vec2(ax,ay))
            .add(pointOnLine)
            .add(normal.clone().scale(r));

          x = restore.x;
          y = restore.y;
          lined = true;
        }
      }
    }

    if (lined) return {
      normal: normal,
      restore: restore
    };

    for (var i = 0; i < lines.length - 1; i++) {
      var delta = new Vec2 (x - lines[i].x, y - lines[i].y);
      if (delta.magnitude2() < r2) {
        var norm = delta.normalize();
        return {
          normal: norm,
          restore: (new Vec2 (lines[i].x,lines[i].y)).add(norm.clone().scale(r))
        }
      }
    }

    return null;
  }

  Collision.prototype._sweepCircleCollision = function (x0,y0,x1,y1,r) {
    var r2 = r * r;
    var dx = x1 - x0;
    var dy = y1 - y0;
    var d2 = dx*dx + dy*dy;

    var col = this._circleCollision (x1,y1,r);
    if (col) return col;
    if (d2 < r2) return null;

    var dist = Math.sqrt (d2);
    var stepx = r * (dx / dist);
    var stepy = r * (dy / dist);

    do {
      x0 += stepx;
      y0 += stepy;

      col = this._circleCollision (x0,y0,r);
      if (col) return col;

      dx = x1 - x0;
      dy = y1 - y0;
    }
    while (dx*dx + dy*dy >= r2);

    return this._circleCollision (x1,y1,r);
  }

  Collision.prototype.step = function (pos,vel,radius) {
    var pos2 = {
      x: pos.x + vel.x,
      y: pos.y + vel.y
    };

    var collisionResult = this._circleCollision (pos2.x,pos2.y,radius);

    if (collisionResult) {
      return {
        pos: collisionResult.restore,
        vel: (new Vec2(vel)).reflect (collisionResult.normal, 0, 1)
      };
    }
    return {
      pos: pos2,
      vel: vel
    };
  }
*/
