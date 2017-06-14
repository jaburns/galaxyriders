using UnityEngine;

namespace Jaburnsnet.CellMapGen
{
    static public class Automaton
    {
        public static bool[,] Generate(
            int width, int height, int seed = 0, float population = .45f, int birth = 5, int survival = 4, int iterations = 12
        ) {
            Random.seed = seed;

            var result = new bool[width,height];
            var buffer = new bool[width,height];

            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y) {
                    if (x == 0 || y == 0 || x == width-1 || y == height-1) {
                        result[x,y] = true;
                        buffer[x,y] = true;
                    }
                    else result[x,y] = Random.value < population;
                }
            }

            for (int i = 0; i < iterations; ++i) {
                for (int x = 1; x < width-1; ++x) {
                    for (int y = 1; y < height-1; ++y) {
                        var neighbors = getNeighborhood(result, x, y);
                        buffer[x,y] = neighbors >= (result[x,y] ? survival : birth);
                    }
                }
                result = buffer;
            }

            return result;
        }

        static int getNeighborhood(bool[,] map, int x, int y)
        {
            int result = 0;

            for (int nx = x-1; nx <= x+1; ++nx) {
                for (int ny = y-1; ny <= y+1; ++ny) {
                    if (nx == x && ny == y) continue;
                    if (nx < 1 || ny < 1 || nx >= map.GetLength(0)-1 || ny >= map.GetLength(1)-1) result++;
                    else result += map[nx,ny] ? 1 : 0;
                }
            }

            return result;
        }
    }
}
