using UnityEngine;

static public class Vector2Ext
{
    static public Vector2 WithX(this Vector2 v, float x) { return new Vector2(x, v.y); }
    static public Vector2 WithY(this Vector2 v, float y) { return new Vector2(v.x, y); }

    static public Vector2 Rotate(this Vector2 v, float degrees)
    {
        return Quaternion.Euler(0, 0, degrees) * v;
    }

    static public Vector3 AsVector3(this Vector2 v, float z = 0.0f)
    {
        return new Vector3(v.x, v.y, z);
    }

    static public Vector2 FromPolar(float radius, float radians)
    {
        return new Vector2 {
            x = radius * Mathf.Cos(radians),
            y = radius * Mathf.Sin(radians)
        };
    }

    static public float Dot(this Vector2 v, Vector2 v1)
    {
        return v.x*v1.x + v.y*v1.y;
    }

    static public float Cross(this Vector2 v, Vector2 v1)
    {
        return v.x * v1.y - v.y * v1.x;
    }

    static public bool VeryNear(this Vector2 v, Vector2 v1)
    {
        return (v - v1).sqrMagnitude < 1e-9;
    }

    /// <summary>
    /// Returns true if the supplied point lies in the space bounded by two infinite lines which intersect the points
    /// defining the provided line segment, and are perpendicular to the provided line segment (a -> b).
    /// </summary>
    static public bool IsInPerpSpaceOfLine(this Vector2 v, Vector2 a, Vector2 b)
    {
        float _ax, _ay;
        float _bx, _by;
        float _cx, _cy;

        float perpSlope = (a.x-b.x)/(b.y-a.y);

        // If the slope is greater than 1, transpose the coordinate space to avoid infinity.
        if (perpSlope > 1) {
            _ax = a.y; _bx = b.y; _cx = v.y;
            _ay = a.x; _by = b.x; _cy = v.x;
            perpSlope = (_ax-_bx)/(_by-_ay);
        } else {
            _ax = a.x; _bx = b.x; _cx = v.x;
            _ay = a.y; _by = b.y; _cy = v.y;
        }

        float yMin, yMax;

        if (_ay > _by) {
            yMin = perpSlope*(_cx - _bx) + _by;
            yMax = perpSlope*(_cx - _ax) + _ay;
        } else {
            yMin = perpSlope*(_cx - _ax) + _ay;
            yMax = perpSlope*(_cx - _bx) + _by;
        }

        return _cy > yMin && _cy < yMax;
    }

    static public Vector2 ProjectOnLine(this Vector2 v, float m)
    {
        const float ACCURACY     = 1000000;
        const float ACCURACY_INV = 1/ACCURACY;

        // Zero slope, horizontal line
        if (Mathf.Abs(m) < ACCURACY_INV) {
            return new Vector2(v.x, 0);
        }

        // Infinite slope, vertical line
        if (Mathf.Abs(m) > ACCURACY) {
            return new Vector2(0, v.y);
        }

        var resultY = (v.y*m+v.x)*m/(1+m*m);
        return new Vector2(resultY / m, resultY);
    }
}

static public class Vector3Ext
{
    static public Vector3 WithX (this Vector3 vec, float x) { return new Vector3 (x, vec.y, vec.z); }
    static public Vector3 WithY (this Vector3 vec, float y) { return new Vector3 (vec.x, y, vec.z); }
    static public Vector3 WithZ (this Vector3 vec, float z) { return new Vector3 (vec.x, vec.y, z); }

    static public Vector3 WithXY (this Vector3 vec, float x, float y) { return new Vector3 (x, y, vec.z); }
    static public Vector3 WithYZ (this Vector3 vec, float y, float z) { return new Vector3 (vec.x, y, z); }
    static public Vector3 WithXZ (this Vector3 vec, float x, float z) { return new Vector3 (x, vec.y, z); }

    static public Vector2 AsVector2(this Vector3 v)
    {
        return new Vector2(v.x, v.y);
    }
}
