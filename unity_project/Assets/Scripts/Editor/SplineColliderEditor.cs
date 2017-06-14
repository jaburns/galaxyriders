using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(SplineCollider))]
public class SplineColliderEditor : Editor
{
    enum EditMode {
        MoveSingle,
        ChangeMode,
        EditQuality,
        Insert,
        Delete
    }

    const float BUTTON_SIZE = .5f;

    SplineCollider targ { get { return target as SplineCollider; } }

    EditMode _editMode = EditMode.MoveSingle;
    int _selectedNode = -1;

    override public void OnInspectorGUI()
    {
        ensureTargInit();

        targ.FlipNormals = EditorGUILayout.Toggle("Flip Normal", targ.FlipNormals);
        targ.SurfaceDepth = EditorGUILayout.FloatField("Surface Depth", targ.SurfaceDepth);

        _editMode = (EditMode)EditorGUILayout.EnumPopup("Edit Mode", _editMode);
        if (_editMode == EditMode.Delete && targ.Points.Count < 4) {
            _editMode = EditMode.MoveSingle;
        }

        if (_selectedNode > -1 && _editMode == EditMode.EditQuality) {
            var oldQuality = targ.Points[_selectedNode].Quality;
            targ.Points[_selectedNode].Quality = EditorGUILayout.IntField("Curve Quality", oldQuality);
            if (targ.Points[_selectedNode].Quality != oldQuality) {
                bake();
            }
        }
    }

    void OnSceneGUI()
    {
        ensureTargInit();

        Vector2 p0 = targ.transform.position;

        for (int i = 0; i < targ.Points.Count; ++i) {
            Handles.color = targ.Points[i].IsCurveHandle ? Color.red : Color.green;
            var clickedNode = Handles.Button(p0 + targ.Points[i].Position, Quaternion.identity, BUTTON_SIZE, BUTTON_SIZE, Handles.SphereCap);

            switch(_editMode) {
                case EditMode.MoveSingle:
                    EditorGUI.BeginChangeCheck();
                    var newPos = Handles.PositionHandle(p0 + targ.Points[i].Position, Quaternion.identity);
                    if (EditorGUI.EndChangeCheck()) {
                        Undo.RecordObject(target, "Moved SplineCollider Node");
                        targ.Points[i].Position = newPos.AsVector2() - p0;
                        bake();
                    }
                    break;
                case EditMode.Insert:
                    if (clickedNode) {
                        Undo.RecordObject(target, "Insert SplineCollider Node");
                        targ.Points.Insert(i+1, SplineCollider.SplineHandle.NewAfter(targ.Points[i]));
                        bake();
                        return;
                    }
                    break;
                case EditMode.EditQuality:
                    if (clickedNode) {
                        _selectedNode = i;
                        return;
                    }
                    break;
                case EditMode.ChangeMode:
                    if (clickedNode) {
                        Undo.RecordObject(target, "Change SplineCollider Node Mode");
                        targ.Points[i].IsCurveHandle = !targ.Points[i].IsCurveHandle;
                        bake();
                        return;
                    }
                    break;
                case EditMode.Delete:
                    if (clickedNode) {
                        Undo.RecordObject(target, "Delete SplineCollider Node");
                        targ.Points.RemoveAt(i);
                        bake();
                        return;
                    }
                    break;
            }
        }

        Handles.color = Color.green;
        for (int i = 0; i < targ.Points.Count - 1; ) {
            if (i <= targ.Points.Count - 4 && targ.Points[i+1].IsCurveHandle && targ.Points[i+2].IsCurveHandle) {
                Handles.DrawBezier(
                    p0 + targ.Points[i+0].Position,
                    p0 + targ.Points[i+3].Position,
                    p0 + targ.Points[i+1].Position,
                    p0 + targ.Points[i+2].Position,
                    Color.green, null, 2
                );
                i += 3;
            } else {
                Handles.DrawLine(
                    p0 + targ.Points[i+0].Position,
                    p0 + targ.Points[i+1].Position
                );
                i += 1;
            }
        }
    }

    void ensureTargInit()
    {
        if (targ.Points != null && targ.Points.Count >= 4) return;

        targ.Points = new List<SplineCollider.SplineHandle> {
            new SplineCollider.SplineHandle(1, 0),
            new SplineCollider.SplineHandle(1, 1),
            new SplineCollider.SplineHandle(2, 1),
            new SplineCollider.SplineHandle(2, 2)
        };
    }

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

    static Vector2 spline(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t)
    {
        var u = 1 - t;
        return u*u*u*a + 3*u*u*t*b + 3*u*t*t*c + t*t*t*d;
    }
}
