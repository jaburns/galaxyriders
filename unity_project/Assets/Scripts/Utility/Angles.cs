using UnityEngine;

static public class Angles
{
    static public float SmallestDifference(float radsA, float radsB)
    {
        radsA %= 2*Mathf.PI;
        radsB %= 2*Mathf.PI;

        if (Mathf.Abs(radsA - radsB) > Mathf.PI) {
            if (radsA > 0) radsA -= 2*Mathf.PI;
            else radsA += 2*Mathf.PI;
        }

        return radsA - radsB;
    }
}
