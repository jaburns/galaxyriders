using System.IO;
using System.Linq;
using Jaburnsnet.CellMapGen;
using UnityEditor;
using UnityEngine;

[CustomEditor(typeof(CellularLevel))]
public class CellularLevelEditor : Editor
{
    CellularLevel targ { get { return target as CellularLevel; } }

    override public void OnInspectorGUI() {
        base.OnInspectorGUI();

        if (GUILayout.Button("Generate")) {
            generate();
        }
    }

    void generate()
    {
        PixelMap map;
        if (targ.LevelMap != null) {
            map = PixelMap.BuildFromTexture(targ.LevelMap);
        } else {
            map = PixelMap.BuildFromBoolMap(Automaton.Generate(102, 102, targ.Seed, targ.Population, targ.Birth, targ.Survival, targ.Iterations));
        }

        var contours = new FindContours(map, targ.SpaceInsurance);
        var tex = map.RenderToTexture(contours.WalkMap);

        File.WriteAllBytes("Assets/CellMapGen/GeneratedPixMap.png",  tex.EncodeToPNG());
        AssetDatabase.Refresh();
        var loadedTex = AssetDatabase.LoadAssetAtPath<Texture2D>("Assets/CellMapGen/GeneratedPixMap.png");

        targ.GetComponent<Renderer>().sharedMaterial.SetTexture("_MainTex", loadedTex);

        foreach (Transform child in targ.transform.Cast<Transform>().ToList()) {
            DestroyImmediate(child.gameObject);
        }

        for (int i = 0; i < contours.Contours.Length; ++i) {
            var smoothed = SmoothCurve.Run(contours.Contours[i], targ.CurveResolution, targ.CurveBend).ToArray();

            var lineObj = new GameObject();
            lineObj.name = "Curve"+i;
            lineObj.transform.parent = targ.transform;
            lineObj.transform.localPosition = Vector3.zero;
            lineObj.transform.localScale = Vector3.one;

            var line = lineObj.AddComponent<LineRenderer>();
            line.sharedMaterial = targ.LineMaterial;
            line.SetWidth(.1f, .1f);
            line.SetVertexCount(smoothed.Length);
            line.SetPositions(smoothed);
            line.useWorldSpace = false;

            var col = lineObj.AddComponent<EdgeCollider2D>();
            col.points = smoothed.Select(x => x.AsVector2()).ToArray();
        }
    }
}
