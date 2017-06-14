using UnityEngine;

namespace Jaburnsnet.CellMapGen
{
    public enum PixelKind {
        Air,
        Dirt,
        Edge,
    }

    public struct PixelData {
        public PixelKind kind;
        public int normal;
    }

    public class PixelMap
    {
        static readonly PixelData OOB_PIXEL = new PixelData {
            kind = PixelKind.Dirt,
            normal = 0,
        };

        readonly PixelData[,] _data;

        public int width  { get { return _data.GetLength(0); } }
        public int height { get { return _data.GetLength(1); } }

        public PixelData this[int x, int y] {
            get {
                return x >= 0 && y >= 0 && x < width && y < height
                    ? _data[x,y]
                    : OOB_PIXEL;
            }
        }

        PixelMap(int width, int height)
        {
            _data = new PixelData[width, height];
        }

        public static PixelMap BuildFromTexture(Texture2D tex)
        {
            var result = new PixelMap(tex.width, tex.height);
            var pixels = tex.GetPixels();

            for (int i = 0; i < pixels.Length; ++i) {
                int x = i % tex.width;
                int y = i / tex.width;
                result._data[x,y] = new PixelData {
                    kind = pixels[i].r < 0.5f ? PixelKind.Dirt : PixelKind.Air
                };
            }

            result.markEdges();
            return result;
        }

        public static PixelMap BuildFromBoolMap(bool[,] boolMap, bool trueIsDirt=true)
        {
            var result = new PixelMap(boolMap.GetLength(0), boolMap.GetLength(1));

            for (int x = 0; x < boolMap.GetLength(0); ++x) {
                for (int y = 0; y < boolMap.GetLength(1); ++y) {
                    result._data[x,y] = new PixelData {
                        kind = (trueIsDirt ? boolMap[x,y] : !boolMap[x,y])
                            ? PixelKind.Dirt : PixelKind.Air
                    };
                }
            }

            result.markEdges();
            return result;
        }

        void markEdges()
        {
            for (var x = 0; x < width; x++) {
                for (var y = 0; y < height; y++) {
                    if (_data[x,y].kind != PixelKind.Dirt) continue;

                    var l = this[x-1,y].kind == PixelKind.Air;
                    var r = this[x+1,y].kind == PixelKind.Air;
                    var u = this[x,y-1].kind == PixelKind.Air;
                    var d = this[x,y+1].kind == PixelKind.Air;

                    if (!l && !r && !u && !d) continue;

                    int normal = r && d ? 45 :
                        d && l ?  135 :
                        l && u ? -135 :
                        u && r ?  -45 :
                        r ? 0 :
                        d ? 90 :
                        l ? 180 :
                        -90;

                    _data[x,y].kind = PixelKind.Edge;
                    _data[x,y].normal = normal;
                }
            }
        }

        public Texture2D RenderToTexture(FindContours.WalkedStatus[,] walkMap = null)
        {
            var texture = new Texture2D(width, height, TextureFormat.ARGB32, false);
            texture.filterMode = FilterMode.Point;

            var pixels = new Color[width * height];
            int i = 0;
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    Color c = Color.magenta;
                    switch(_data[x,y].kind) {
                        case PixelKind.Air:
                            c = new Color(0,0,0,0);
                            break;
                        case PixelKind.Dirt:
                            c = Color.black;
                            break;
                        case PixelKind.Edge:
                            c = new Color(.2f, 0, 0, 1);
                            break;
                    }
                    if (walkMap != null) {
                        switch(walkMap[x,y]) {
                            case FindContours.WalkedStatus.Walked:
                                c = new Color(0, 0, .2f, 1);
                                break;
                            case FindContours.WalkedStatus.WalkedImportant:
                                c = new Color(0, .15f, 0, 1);
                                break;
                        }
                    }
                    pixels[i++] = c;
                }
            }

            texture.SetPixels(pixels);
            texture.Apply();
            return texture;
        }
    }
}
