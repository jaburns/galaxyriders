using System;
using UnityEngine;

public class WorldPaletteLoader : MonoBehaviour
{
    public WorldPalette[] Palettes;

    public Material TwoColorGround;
    public Material SurfaceMaterial;
    public Material GroundMaterial;
    public Material LivingMaterial;

    public int StartingIndex;

    public GameEvent OnChange;

    WorldPalette _current;

    public WorldPalette Palette { get { return _current; } }

    static WorldPaletteLoader s_instance;
    static public WorldPaletteLoader Instance {
        get {
            if (s_instance == null) {
                s_instance = FindObjectOfType<WorldPaletteLoader>();
                if (s_instance.OnChange == null) {
                    s_instance.OnChange = new GameEvent();
                }
            }
            return s_instance;
        }
    }

    void Awake()
    {
        _current = Palettes[StartingIndex];
        updateMaterials();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.P)) {
            var i = Array.IndexOf(Palettes, _current);
            _current = Palettes[(i+1) % Palettes.Length];
            updateMaterials();
        }
    }

    void updateMaterials()
    {
        TwoColorGround.SetColor("_ColorA", _current.SurfaceColor);
        TwoColorGround.SetColor("_ColorB", _current.GroundColor);
        SurfaceMaterial.color = _current.SurfaceColor;
        GroundMaterial.color = _current.GroundColor;
        LivingMaterial.color = _current.LifeColor;
        OnChange.Broadcast();
    }
}
