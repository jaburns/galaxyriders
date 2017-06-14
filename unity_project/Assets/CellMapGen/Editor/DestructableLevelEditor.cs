using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(DestructableLevel))]
public class DestructableLevelEditor : Editor
{
    DestructableLevel targ { get { return target as DestructableLevel; } }

    void OnSceneGUI()
    {

    }

    override public void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        if (GUILayout.Button("Preview Seed")) {
            targ.LoadStateFromSeed();
            targ.UpdateFromState();
        }
    }
}
