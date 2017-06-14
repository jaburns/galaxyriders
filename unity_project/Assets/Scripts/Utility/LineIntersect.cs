using UnityEngine;

static public class LineIntersect
{
    /// <summary>
    /// Given the coordinates of the end points of two line segments, this function returns
    /// their intersection point, or null if they do not intersect.
    /// </summary>
    static public Vector2? Test(Vector2 p00, Vector2 p01, Vector2 p10, Vector2 p11)
    {
        var dx1x3 = p00.x - p10.x;
        var dy1y3 = p00.y - p10.y;
        var dx2x1 = p01.x - p00.x;
        var dy2y1 = p01.y - p00.y;
        var dx4x3 = p11.x - p10.x;
        var dy4y3 = p11.y - p10.y;

        var denom = dy4y3*dx2x1 - dx4x3*dy2y1;
        var numa  = dx4x3*dy1y3 - dy4y3*dx1x3;
        var numb  = dx2x1*dy1y3 - dy2y1*dx1x3;

        if (denom*denom < Mathf.Epsilon) return null;

        numa /= denom;
        numb /= denom;

        if (numa >= 0 && numa <= 1 && numb >= 0 && numb <= 1) {
            return new Vector2(p00.x + dx2x1*numa, p00.y + dy2y1*numa);
        }

        return null;
    }
}
