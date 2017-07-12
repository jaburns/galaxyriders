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

/*
    Vector2[] insetPoints(Vector2[] points)
    {
        var ret = new Vector2[points.Length];

        for (int i = 0; i < points.Length; i ++) {
            var i0 = i == 0 ? points.Length - 1 : i - 1;
            var i1 = i == points.Length - 1 ? 0 : i + 1;
            Vector2 a = points[i0], b = points[i], c = points[i1];

            var norm = ((b - a).normalized + (b - c).normalized).normalized;
            if (((b - a).Cross(c - a) < 0) != targ.FlipNormals) norm *= -1;

            ret[i] = b + norm * targ.SurfaceDepth;
        }

        return ret;
    }

    void bake()
    {
        var points = generatePoints();

        var edge = targ.gameObject.EnsureComponent<EdgeCollider2D>();
        edge.points = points.ToArray();

        var originalVerts = points.ToArray();
        var insetVerts = insetPoints(originalVerts);

        var vertices = new Vector3[2 * originalVerts.Length];
        var uvs = new Vector2[2 * originalVerts.Length];
        for (int i = 0; i < vertices.Length; i++) {
            vertices[i] = i % 2 == 0 ? originalVerts[i/2] : insetVerts[i/2];
            uvs[i] = i % 2 == 0 ? Vector2.zero : Vector2.one;
        }

        int[] innerIndices = (new Triangulator(insetVerts)).Triangulate();

        var indices = new int[3 * vertices.Length + innerIndices.Length];
        for (int i = 0; i < vertices.Length; i += 2) {
            var a = i;
            var b = i+1;
            var c = (i+2)%vertices.Length;
            var d = (i+3)%vertices.Length;

            indices[3*i  ] = targ.FlipNormals ? b : a;
            indices[3*i+1] = targ.FlipNormals ? c : c;
            indices[3*i+2] = targ.FlipNormals ? a : b;
            indices[3*i+3] = targ.FlipNormals ? d : b;
            indices[3*i+4] = targ.FlipNormals ? c : c;
            indices[3*i+5] = targ.FlipNormals ? b : d;
        }
        for (int i = 0; i < innerIndices.Length; ++i) {
            indices[3*vertices.Length + i] = 2*innerIndices[i] + 1;
        }

        var mesh = new Mesh();
        mesh.vertices = vertices;
        mesh.triangles = indices;
        mesh.uv = uvs;
        mesh.RecalculateNormals();
        mesh.RecalculateBounds();

        var filter = targ.gameObject.EnsureComponent<MeshFilter>();
        filter.sharedMesh = mesh;

        targ.gameObject.EnsureComponent<MeshRenderer>();
    }
*/

LevelRenderer::Mesh LevelRenderer::load_mesh(const BakedLevel& level)
{
    LevelRenderer::Mesh mesh;

    for (auto& poly : level.polys) {
        for (auto& pt : poly.points) {
            mesh.vertices.push_back(fixed32::to_float(pt));
            mesh.vdepths.push_back(0.0f);
        }
    }

    uint32_t base_index = 0;
    for (auto& poly : level.polys) {
        auto tris = Triangulator::triangulate(poly.points);
        for (auto tri : tris) {
            mesh.indices.push_back(base_index + tri);
        }
        base_index += poly.points.size();
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
