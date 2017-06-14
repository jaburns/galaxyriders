using UnityEngine;

public class CellularLevel : MonoBehaviour
{
    public Texture2D LevelMap;
    public Material LineMaterial;

    public int Seed;
    [Range(0f, 1f)]
    public float Population = .45f;
    [Range(0, 8)]
    public int Birth = 5;
    [Range(0, 8)]
    public int Survival = 4;
    [Range(0, 20)]
    public int Iterations = 12;

    [Range(0, 2)]
    public int SpaceInsurance;
    [Range(1, 20)]
    public int CurveResolution = 10;
    [Range(0f, 3f)]
    public float CurveBend = 1f;
}
