#include "triangulator.hpp"
#include <algorithm>

static fixed32 area(const std::vector<fixed32::vec2>& points)
{
    size_t n = points.size();
    fixed32 A = fixed32::ZERO;
    for (size_t p = n - 1, q = 0; q < n; p = q++) {
        fixed32::vec2 pval = points[p];
        fixed32::vec2 qval = points[q];
        A += pval.x * qval.y - qval.x * pval.y;
    }
    return A / fixed32::TWO;
}

static bool inside_triangle(fixed32::vec2 A, fixed32::vec2 B, fixed32::vec2 C, fixed32::vec2 P)
{
    fixed32 ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
    fixed32 cCROSSap, bCROSScp, aCROSSbp;

    ax = C.x - B.x; ay = C.y - B.y;
    bx = A.x - C.x; by = A.y - C.y;
    cx = B.x - A.x; cy = B.y - A.y;
    apx = P.x - A.x; apy = P.y - A.y;
    bpx = P.x - B.x; bpy = P.y - B.y;
    cpx = P.x - C.x; cpy = P.y - C.y;

    aCROSSbp = ax * bpy - ay * bpx;
    cCROSSap = cx * apy - cy * apx;
    bCROSScp = bx * cpy - by * cpx;

    return ((aCROSSbp >= fixed32::ZERO) && (bCROSScp >= fixed32::ZERO) && (cCROSSap >= fixed32::ZERO));
}

static bool snip(const std::vector<fixed32::vec2>& points, size_t u, size_t v, size_t w, size_t n, const std::vector<size_t>& V)
{
    fixed32::vec2 A = points[V[u]];
    fixed32::vec2 B = points[V[v]];
    fixed32::vec2 C = points[V[w]];

    fixed32 pdiff = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
    if (pdiff.to_raw_int() < 2) return false;

    for (size_t p = 0; p < n; p++) {
        if ((p == u) || (p == v) || (p == w)) continue;
        fixed32::vec2 P = points[V[p]];
        if (inside_triangle(A, B, C, P)) return false;
    }
    return true;
}

std::vector<uint32_t> Triangulator::triangulate(const std::vector<fixed32::vec2>& pts)
{
    std::vector<fixed32::vec2> points = pts;
    std::vector<uint32_t> indices;

    size_t n = points.size();
    if (n < 3) return indices;

    std::vector<size_t> V(n);
    if (area(points) > 0) {
        for (size_t v = 0; v < n; v++) V[v] = v;
    } else {
        for (size_t v = 0; v < n; v++) V[v] = (n - 1) - v;
    }

    auto nv = n;
    auto count = 2 * nv;
    for (size_t m = 0, v = nv - 1; nv > 2; ) {
        if ((count--) <= 0) return indices;

        auto u = v;
        if (nv <= u) u = 0;
        v = u + 1;
        if (nv <= v) v = 0;
        auto w = v + 1;
        if (nv <= w) w = 0;

        if (snip(points, u, v, w, nv, V)) {
            indices.push_back(V[u]);
            indices.push_back(V[v]);
            indices.push_back(V[w]);
            m++;
            for (size_t s = v, t = v + 1; t < nv; s++, t++) V[s] = V[t];
            nv--;
            count = 2 * nv;
        }
    }

    std::reverse(indices.begin(), indices.end());
    return indices;
}
