#include "level_mesh.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <vector>
#include <algorithm>
#include "../../shared/geometry.hpp"
#include "../triangulator.hpp"

static std::vector<glm::vec2> inset_points(const std::vector<glm::vec2>& points)
{
    static constexpr auto DEPTH = 5.0f;

    std::vector<glm::vec2> result(points.size());

    for (auto i = 0; i < points.size(); ++i) {
        const auto i0 = i == 0 ? points.size() - 1 : i - 1;
        const auto i1 = i == points.size() - 1 ? 0 : i + 1;
        const auto a = points[i0], b = points[i], c = points[i1];

        const auto b_a = glm::normalize(b - a);
        const auto c_b = glm::normalize(c - b);

        const auto half_vector = glm::normalize(b_a + c_b);
        const auto diff_dot = glm::dot(b_a, half_vector);
        const auto depth = DEPTH / diff_dot;

        auto norm = glm::normalize(b_a - c_b);
        if (Geometry::vec2_cross(b_a, c - a) < 0.0f) norm *= -1;

        result[i] = b + norm * depth;
    }

    return result;
}

static void push_poly(LevelMeshRenderer::Mesh& mesh, const BakedLevel::Poly& poly)
{
    const auto inset_pts = inset_points(poly.points);

    // ----- Inner-earth mesh building -----
    {
        const auto inner_indices = Triangulator::triangulate(inset_pts);

        const auto base_vert_index = mesh.vertices.size();
        const auto base_tri_index = mesh.indices.size();

        mesh.vertices.reserve(base_vert_index + inset_pts.size());
        mesh.surface_info.reserve(base_vert_index + inset_pts.size());
        mesh.indices.reserve(base_tri_index + inner_indices.size());

        for (auto i = 0; i < inset_pts.size(); ++i) {
            mesh.vertices.push_back(inset_pts[i]);
            mesh.surface_info.push_back({ 1.0f, 0.0f, 1.0f });
        }

        for (auto i = 0; i < inner_indices.size(); ++i) {
            mesh.indices.push_back(base_vert_index + inner_indices[i]);
        }
    }

    // ----- Outer crust mesh building
    {
        const auto base_vert_index = mesh.vertices.size();
        const auto base_tri_index = mesh.indices.size();
        const auto new_vert_count = 4 * poly.points.size();

        mesh.vertices.reserve(base_vert_index + new_vert_count);
        mesh.surface_info.reserve(base_vert_index + new_vert_count);
        mesh.indices.reserve(base_tri_index + 6 * poly.points.size());

        for (auto i = 0; i < poly.points.size(); i++) {
            const auto j = (i + 1) % poly.points.size();

            mesh.vertices.push_back(poly.points[i]);
            mesh.vertices.push_back(inset_pts[i]);
            mesh.vertices.push_back(poly.points[j]);
            mesh.vertices.push_back(inset_pts[j]);

            const auto surface_vec = poly.points[j] - poly.points[i];
            const auto tangent = glm::normalize(surface_vec);
            const auto normal = Geometry::vec2_rotate90(tangent);

            mesh.surface_info.push_back(glm::vec3(normal, 0.0f));
            mesh.surface_info.push_back(glm::vec3(normal, 1.0f));
            mesh.surface_info.push_back(glm::vec3(normal, 0.0f));
            mesh.surface_info.push_back(glm::vec3(normal, 1.0f));
        }

        for (auto i = 0; i < new_vert_count; i += 4) {
            const auto a = base_vert_index + i;
            const auto b = base_vert_index + i+1;
            const auto c = base_vert_index + (i+2)%new_vert_count;
            const auto d = base_vert_index + (i+3)%new_vert_count;

            mesh.indices.push_back(a);
            mesh.indices.push_back(c);
            mesh.indices.push_back(b);
            mesh.indices.push_back(b);
            mesh.indices.push_back(c);
            mesh.indices.push_back(d);
        }
    }
}

static LevelMeshRenderer::Mesh create_mesh(const BakedLevel& level)
{
    LevelMeshRenderer::Mesh result;

    for (const auto& poly : level.polys) {
        push_poly(result, poly);
    }

    return result;
}

LevelMeshRenderer::LevelMeshRenderer(const Level& level)
:   m_mesh(create_mesh(BakedLevel::from_level(level))),
    m_program("res/shaders/level.vert", "res/shaders/level.frag"),
    m_noise_texture("res/textures/noise.png")
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.vertices.size() * sizeof(glm::vec2), m_mesh.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(m_program, "position"), 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*) 0);

    glGenBuffers(1, &m_surface_pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_surface_pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.surface_info.size() * sizeof(glm::vec3), m_mesh.surface_info.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_program, "surface_info"));
    glVertexAttribPointer(glGetAttribLocation(m_program, "surface_info"), 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.indices.size() * sizeof(uint32_t), m_mesh.indices.data(), GL_STATIC_DRAW);
}

LevelMeshRenderer::~LevelMeshRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_surface_pos_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}

void LevelMeshRenderer::draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_noise_texture);

    glUseProgram(m_program);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(m_program, "noise_texture"), 0);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    const auto m = glm::translate(glm::mat4(1.0f), position);
    glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, m_mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
}
