#include "level.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "../triangulator.hpp"
#include "../palette.hpp"

LevelRenderer::LevelRenderer(const BakedLevel& level)
{
    m_program = std::make_unique<const ShaderProgram>("res/shaders/level.vert", "res/shaders/level.frag");
    m_noise_texture = std::make_unique<const Texture>("res/textures/noise.png");
    m_ground_texture = std::make_unique<const Texture>("res/textures/ground.png");
    m_mesh = load_mesh(level);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.vertices.size() * sizeof(glm::vec2), m_mesh.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "position"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "position"), 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);

    glGenBuffers(1, &m_vdepth_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vdepth_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh.vdepths.size() * sizeof(float), m_mesh.vdepths.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(*m_program, "vdepth"));
    glVertexAttribPointer(glGetAttribLocation(*m_program, "vdepth"), 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*) 0);

    glGenBuffers(1, &m_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh.indices.size() * sizeof(uint32_t), m_mesh.indices.data(), GL_STATIC_DRAW);
}

LevelRenderer::~LevelRenderer()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_vdepth_buffer);
    glDeleteBuffers(1, &m_index_buffer);
}

static float cross2(glm::vec2 a, glm::vec2 b)
{
    return (a.x*b.y) - (a.y*b.x);
}

static const auto DEPTH = 1.0f;

static std::vector<glm::vec2> inset_points(const std::vector<glm::vec2>& points)
{
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
        if (cross2(b_a, c - a) < 0.0f) norm *= -1;

        result[i] = b + norm * depth;
    }

    return result;
}

void LevelRenderer::push_poly(Mesh& mesh, const BakedLevel::Poly& poly)
{
    size_t base_vert_index = mesh.vertices.size();
    size_t base_tri_index = mesh.indices.size();

    auto inset_pts = inset_points(poly.points);
    auto new_vert_count = 2 * poly.points.size();

    mesh.vertices.reserve(base_vert_index + new_vert_count);
    mesh.vdepths.reserve(base_vert_index + new_vert_count);

    for (auto i = 0; i < new_vert_count; i++) {
        mesh.vertices.push_back(i % 2 == 0 ? poly.points[i/2] : inset_pts[i/2]);
        mesh.vdepths.push_back(i % 2 == 0 ? 0.0f : 1.0f);
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

LevelRenderer::Mesh LevelRenderer::load_mesh(const BakedLevel& level)
{
    LevelRenderer::Mesh mesh;

    for (auto& poly : level.polys) {
        push_poly(mesh, poly);
    }

    return mesh;
}

void LevelRenderer::draw_once(const glm::mat4x4& view, const glm::mat4x4& projection, const glm::vec3& position)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *m_noise_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *m_ground_texture);

    glUseProgram(*m_program);
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(*m_program, "noise_texture"), 0);
    glUniform1i(glGetUniformLocation(*m_program, "ground_texture"), 1);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    auto m = glm::translate(glm::mat4(1.0f), position);

    glUniformMatrix4fv(glGetUniformLocation(*m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, m_mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
}
