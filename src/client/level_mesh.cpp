#include "level_mesh.hpp"

#include <glm/geometric.hpp>
#include <vector>
#include <algorithm>
#include "../shared/geometry.hpp"
#include "triangulator.hpp"

static std::vector<glm::vec2> inset_points(const std::vector<glm::vec2>& points)
{
    static constexpr auto DEPTH = 1.0f;

    std::vector<glm::vec2> result(points.size());

    for (size_t i = 0; i < points.size(); ++i) {
        auto i0 = i == 0 ? points.size() - 1 : i - 1;
        auto i1 = i == points.size() - 1 ? 0 : i + 1;
        glm::vec2 a = points[i0], b = points[i], c = points[i1];

        auto b_a = glm::normalize(b - a);
        auto c_b = glm::normalize(c - b);

        auto half_vector = glm::normalize(b_a + c_b);
        auto diff_dot = glm::dot(b_a, half_vector);
        auto depth = DEPTH / diff_dot;

        auto norm = glm::normalize(b_a - c_b);
        if (Geometry::vec2_cross(b_a, c - a) < 0.0f) norm *= -1;

        result[i] = b + norm * depth;
    }

    return result;
}

static void push_poly(LevelMesh& mesh, const BakedLevel::Poly& poly)
{
    size_t base_vert_index = mesh.vertices.size();
    size_t base_tri_index = mesh.indices.size();

    auto inset_pts = inset_points(poly.points);
    auto new_vert_count = 2 * poly.points.size();

    mesh.vertices.reserve(base_vert_index + new_vert_count);
    mesh.surface_pos.reserve(base_vert_index + new_vert_count);

    float surface_run = 0.0f;

    for (auto i = 0; i < new_vert_count; i++) {
        if (i > 0 && i % 2 == 0) {
            surface_run += glm::length(poly.points[i/2] - poly.points[i/2-1]);
        }

        mesh.vertices.push_back(i % 2 == 0 ? poly.points[i/2] : inset_pts[i/2]);
        mesh.surface_pos.push_back(glm::vec2(
            surface_run,
            i % 2 == 0 ? 0.0f : 1.0f
        ));
    }

    std::vector<uint32_t> inner_indices = Triangulator::triangulate(poly.points);

    mesh.indices.resize(base_tri_index + 3 * new_vert_count + inner_indices.size());

    for (auto i = 0; i < new_vert_count; i += 2) {
        auto a = base_tri_index + i;
        auto b = base_tri_index + i+1;
        auto c = base_tri_index + (i+2)%new_vert_count;
        auto d = base_tri_index + (i+3)%new_vert_count;

        mesh.indices[3*i  ] = a;
        mesh.indices[3*i+1] = c;
        mesh.indices[3*i+2] = b;
        mesh.indices[3*i+3] = b;
        mesh.indices[3*i+4] = c;
        mesh.indices[3*i+5] = d;
    }

    for (int i = 0; i < inner_indices.size(); ++i) {
        mesh.indices[3*new_vert_count + i] = base_tri_index + 2*inner_indices[i] + 1;
    }
}

LevelMesh::LevelMesh(const BakedLevel& level)
{
    for (const auto& poly : level.polys) {
        push_poly(*this, poly);
    }
}