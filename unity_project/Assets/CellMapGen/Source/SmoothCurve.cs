using UnityEngine;
using System.Collections.Generic;

namespace Jaburnsnet.CellMapGen
{
    static public class SmoothCurve
    {
        // http://www.antigrain.com/research/bezier_interpolation/

        struct Pt {
            public Vector3 pt;
            public Vector3 controlA;
            public Vector3 controlB;
        }

        static public List<Vector3> Run(IList<Vector3> input, int resolution = 10, float curve = 1f)
        {
            var pts = new List<Pt>(input.Count);

            if (resolution < 1) resolution = 1;

            for (int i = 0; i < input.Count; ++i) {
                var prev = input[i == 0 ? input.Count-1 : i-1];
                var curr = input[i];
                var next = input[(i + 1) % input.Count];

                var lenA = (prev - curr).magnitude;
                var lenB = (curr - next).magnitude;

                var midA = (prev + curr) / 2;
                var midB = (curr + next) / 2;
                var midDiff = (midA - midB) * curve;

                var diffA =  midDiff * lenA / (lenA + lenB);
                var diffB = -midDiff * lenB / (lenA + lenB);

                pts.Add(new Pt {
                    pt = curr,
                    controlA = curr + diffA,
                    controlB = curr + diffB,
                });
            }

            var inc = 1 / (float)resolution;

            var result = new List<Vector3>(resolution*input.Count);
            for (int i = 0; i < pts.Count; ++i) {
                var curr = pts[i];
                var next = pts[(i + 1) % pts.Count];

                for (float t = 0; t < 1; t += inc) {
                    result.Add(
                            (1-t)*(1-t)*(1-t)*curr.pt
                        + 3*(1-t)*(1-t)*t*curr.controlB
                        + 3*(1-t)*t*t*next.controlA
                        + t*t*t*next.pt
                    );
                }
            }

            return result;
        }
    }
}
