#include "level.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "../triangulator.hpp"

LevelRenderer::LevelRenderer(const BakedLevel& level)
{
    m_program = std::make_unique<const ShaderProgram>("res/shaders/level.vert", "res/shaders/level.frag");
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

static std::vector<glm::vec2> inset_points(const std::vector<glm::vec2>& points)
{
    const auto DEPTH = 0.1f;
    std::vector<glm::vec2> result(points.size());

    for (size_t i = 0; i < points.size(); ++i) {
        auto i0 = i == 0 ? points.size() - 1 : i - 1;
        auto i1 = i == points.size() - 1 ? 0 : i + 1;
        glm::vec2 a = points[i0], b = points[i], c = points[i1];

        auto b_a = glm::normalize(b - a);
        auto b_c = glm::normalize(b - c);
        auto norm = glm::normalize(b_a + b_c);

        result[i] = b + norm * DEPTH;
    }

    return result;
}

void LevelRenderer::push_poly(Mesh& mesh, const BakedLevel::Poly& poly)
{
//  size_t base_index = mesh.vertices.size();
    std::vector<glm::vec2> points(poly.points.size());

    for (auto i = 0; i < points.size(); ++i) {
        points[i] = fixed32::to_float(points[i]);
    }

    // ###  Implementation with inset ### //
//  auto inset_pts = inset_points(points);
//  var vertices = new Vector3[2 * originalVerts.Length];
//  var uvs = new Vector2[2 * originalVerts.Length];
//  for (int i = 0; i < vertices.Length; i++) {
//      vertices[i] = i % 2 == 0 ? originalVerts[i/2] : insetVerts[i/2];
//      uvs[i] = i % 2 == 0 ? Vector2.zero : Vector2.one;
//  }

//  int[] innerIndices = (new Triangulator(insetVerts)).Triangulate();

//  var indices = new int[3 * vertices.Length + innerIndices.Length];
//  for (int i = 0; i < vertices.Length; i += 2) {
//      var a = i;
//      var b = i+1;
//      var c = (i+2)%vertices.Length;
//      var d = (i+3)%vertices.Length;

//      indices[3*i  ] = targ.FlipNormals ? b : a;
//      indices[3*i+1] = targ.FlipNormals ? c : c;
//      indices[3*i+2] = targ.FlipNormals ? a : b;
//      indices[3*i+3] = targ.FlipNormals ? d : b;
//      indices[3*i+4] = targ.FlipNormals ? c : c;
//      indices[3*i+5] = targ.FlipNormals ? b : d;
//  }
//  for (int i = 0; i < innerIndices.Length; ++i) {
//      indices[3*vertices.Length + i] = 2*innerIndices[i] + 1;
//  }

    // ###  Implementation without inset ### //
    for (auto i = 0; i < points.size(); ++i) {
        mesh.vertices.push_back(points[i]);
        mesh.vdepths.push_back(0.0f);
    }
    auto tris = Triangulator::triangulate(points);
    for (auto tri : tris) {
        mesh.indices.push_back(tri);
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
    glUseProgram(*m_program);
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(*m_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    auto m = glm::translate(glm::mat4(1.0f), position);

    glUniformMatrix4fv(glGetUniformLocation(*m_program, "model"), 1, GL_FALSE, glm::value_ptr(m));

    glDrawElements(GL_TRIANGLES, m_mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);
}
