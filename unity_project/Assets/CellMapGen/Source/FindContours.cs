using UnityEngine;
using System.Collections.Generic;

namespace Jaburnsnet.CellMapGen
{
    public class FindContours
    {
        public enum WalkedStatus {
            Unwalked,
            Walked,
            WalkedImportant,
        }

        struct WalkCandidate {
            public int dx, dy, normal;
        }

        struct FreshStart {
            public int x, y;
        }

        public Vector3[][] Contours { get; private set; }
        public WalkedStatus[,] WalkMap { get; private set; }

        public FindContours(PixelMap map, int spaceInsurance = 2)
        {
            const int MAX_ITER = 100;

            var walkMap = new WalkedStatus[map.width, map.height];
            var contours = new List<Vector3[]>();

            int iter = 0;
            Vector3[] newContour;

            do {
                newContour = findOneContour(walkMap, map, spaceInsurance);
                if (newContour != null && newContour.Length > 2) {
                    contours.Add(newContour);
                }
            } while (newContour != null && iter++ < MAX_ITER);

            Contours = contours.ToArray();
            WalkMap = walkMap;
        }

        static FreshStart? findFreshContour(WalkedStatus[,] walkMap, PixelMap map)
        {
            for (var x = 0; x < map.width; x++) {
                for (var y = 0; y < map.height; y++) {
                    if (map[x,y].kind == PixelKind.Edge && walkMap[x,y] == WalkedStatus.Unwalked) {
                        return new FreshStart { x = x, y = y };
                    }
                }
            }

            return null;
        }

        // spaceInsurance: increased value spreads out contour control points. 0, 1, or 2
        static Vector3[] findOneContour(WalkedStatus[,] walkMap, PixelMap map, int spaceInsurance = 2)
        {
            const int MAX_ITER = 5000;

            var freshStart = findFreshContour(walkMap, map);
            if (!freshStart.HasValue) {
                return null;
            }

            var points = new List<Vector3>();

            var x = freshStart.Value.x;
            var y = freshStart.Value.y;

            var iter = 0;
            var lastCan = new WalkCandidate { dx=0, dy=0, normal=0 };
            var lastStatus = WalkedStatus.Walked;
            var candidates = new List<WalkCandidate>();

            do {
                candidates.Clear();

                for (var dx = -1; dx <= 1; dx++) {
                    for (var dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        if (map[x+dx,y+dy].kind == PixelKind.Edge
                        && walkMap[x+dx,y+dy] == WalkedStatus.Unwalked) {
                            candidates.Add(new WalkCandidate {dx=dx, dy=dy, normal=map[x+dx,y+dy].normal});
                        }
                    }
                }

                if (candidates.Count > 0) {
                    var c = bestCandidate(lastCan, candidates);
                    x += c.dx;
                    y += c.dy;

                    var newStatus = WalkedStatus.Walked;
                    if (c.normal != lastCan.normal) {
                        bool important;
                        if (spaceInsurance <= 0) {
                            important = true;
                        } else if (spaceInsurance == 1) {
                            important = c.dx != 0 && c.dy != 0 || lastStatus != WalkedStatus.WalkedImportant;
                        } else { // spaceInsurance >= 2
                            important = lastStatus != WalkedStatus.WalkedImportant;
                        }
                        if (important) {
                            newStatus = WalkedStatus.WalkedImportant;
                        }
                    }

                    if (newStatus == WalkedStatus.WalkedImportant) {
                        points.Add(new Vector3((float)x/map.width - .5f, (float)y/map.height - .5f, -1f));
                    }

                    walkMap[x,y] = newStatus;
                    lastStatus = newStatus;
                    lastCan = c;
                }
            }
            while (candidates.Count > 0 && iter++ < MAX_ITER);

            return points.ToArray();
        }

        static WalkCandidate bestCandidate(WalkCandidate prev, IList<WalkCandidate> candidates)
        {
            var minVal = 100f;
            var minI = 0;

            for (int i = 0; i < candidates.Count; ++i) {
                var c = candidates[i];
                var d = Mathf.Abs(Angles.SmallestDifference(
                    c.normal * Mathf.Deg2Rad, prev.normal * Mathf.Deg2Rad
                ));

                if (d < minVal) {
                    minVal = d;
                    minI = i;
                }
            }

            return candidates[minI];
        }
    }
}
