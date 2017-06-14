using System.Linq;
using UnityEngine;
using Jaburnsnet.CellMapGen;

public class DestructableLevel : MonoBehaviour
{
    public bool[,] State;
    public int Seed;
    public Material LineMaterial;

    void Awake()
    {
        LoadStateFromSeed();
        UpdateFromState();
    }

    public void LoadStateFromSeed()
    {
        State = Automaton.Generate(102, 102, Seed, 0.51f);
    }

    void removeDanglers()
    {
        for (int x = 1; x < State.GetLength(0)-1; x++) {
            for (int y = 1; y < State.GetLength(1)-1; y++) {
                var airAcrossA = !State[x-1,y-1] && !State[x+1,y+1];
                var dirtAcrossA = State[x-1,y-1] && State[x+1,y+1];
                var airAcrossB = !State[x-1,y+1] && !State[x+1,y-1];
                var dirtAcrossB = State[x-1,y+1] && State[x+1,y-1];

                if (!State[x-1,y] && !State[x+1,y] || !State[x,y-1] && !State[x,y+1]
                ||  airAcrossA && dirtAcrossB || dirtAcrossA && airAcrossB) {
                    State[x,y] = false;
                }
            }
        }
    }

    public void UpdateFromState()
    {
        var map = PixelMap.BuildFromBoolMap(State);
        var contours = new FindContours(map);
        var tex = map.RenderToTexture(contours.WalkMap);

        GetComponent<Renderer>().sharedMaterial.SetTexture("_MainTex", tex);

        foreach (Transform child in transform.Cast<Transform>().ToList()) {
            DestroyImmediate(child.gameObject);
        }

        for (int i = 0; i < contours.Contours.Length; ++i) {
            var smoothed = SmoothCurve.Run(contours.Contours[i]).ToArray();

            var lineObj = new GameObject();
            lineObj.name = "Curve"+i;
            lineObj.transform.parent = transform;
            lineObj.transform.localPosition = Vector3.zero;
            lineObj.transform.localScale = Vector3.one;

            var line = lineObj.AddComponent<LineRenderer>();
            line.sharedMaterial = LineMaterial;
            line.SetWidth(.1f, .1f);
            line.SetVertexCount(smoothed.Length);
            line.SetPositions(smoothed);
            line.useWorldSpace = false;

            var col = lineObj.AddComponent<EdgeCollider2D>();
            col.points = smoothed.Select(x => x.AsVector2()).ToArray();
        }
    }

    public void Nuke(Vector2 worldPos)
    {
        int x = (int)((worldPos.x + 250f/2f) / 250f * 102f);
        int y = (int)((worldPos.y + 250f/2f) / 250f * 102f);
        if (x < 0 || y < 0 || x >= State.GetLength(0) || y >= State.GetLength(1)) return;

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                State[x+dx,y+dy] = false;
            }
        }

        removeDanglers();
        UpdateFromState();
    }
}
