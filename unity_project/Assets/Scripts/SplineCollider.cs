using System;
using System.Collections.Generic;
using UnityEngine;

public class SplineCollider : MonoBehaviour
{
    [Serializable]
    public class SplineHandle
    {
        public Vector2 Position;
        public bool IsCurveHandle;
        public int Quality;

        public SplineHandle(float x, float y)
        {
            Position = new Vector2(x, y);
            IsCurveHandle = false;
            Quality = 10;
        }

        static public SplineHandle NewAfter(SplineHandle s)
        {
            return new SplineHandle(s.Position.x + 1, s.Position.y + 1);
        }
    }

    public List<SplineHandle> Points;
    public bool FlipNormals;
    public float SurfaceDepth;
}
