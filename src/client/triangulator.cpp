#include "triangulator.hpp"
#include <algorithm>

static float area(const std::vector<glm::vec2>& points)
{
    const auto n = points.size();
    auto A = 0.0f;
    for (size_t p = n - 1, q = 0; q < n; p = q++) {
        glm::vec2 pval = points[p];
        glm::vec2 qval = points[q];
        A += pval.x * qval.y - qval.x * pval.y;
    }
    return A / 2.0f;
}

static bool inside_triangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P)
{
    const auto ax = C.x - B.x, ay = C.y - B.y;
    const auto bx = A.x - C.x, by = A.y - C.y;
    const auto cx = B.x - A.x, cy = B.y - A.y;
    const auto apx = P.x - A.x, apy = P.y - A.y;
    const auto bpx = P.x - B.x, bpy = P.y - B.y;
    const auto cpx = P.x - C.x, cpy = P.y - C.y;

    const auto aCROSSbp = ax * bpy - ay * bpx;
    const auto cCROSSap = cx * apy - cy * apx;
    const auto bCROSScp = bx * cpy - by * cpx;

    return aCROSSbp >= 0.0f && bCROSScp >= 0.0f && cCROSSap >= 0.0f;
}

static bool snip(const std::vector<glm::vec2>& points, size_t u, size_t v, size_t w, size_t n, const std::vector<size_t>& V)
{
    const auto A = points[V[u]];
    const auto B = points[V[v]];
    const auto C = points[V[w]];

    const auto pdiff = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
    if (pdiff < 1e-7) return false;

    for (size_t p = 0; p < n; p++) {
        if ((p == u) || (p == v) || (p == w)) continue;
        const auto P = points[V[p]];
        if (inside_triangle(A, B, C, P)) return false;
    }
    return true;
}

std::vector<uint32_t> Triangulator::triangulate(const std::vector<glm::vec2>& pts)
{
    auto points = pts;
    std::vector<uint32_t> indices;

    const auto n = points.size();
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