#include "wire_meshes.hpp"
    
static float BOX_VERTICES_points[] = {
0.0f, 0.0f, 0.0f, 
0.0f, 1.0f, 0.0f, 
0.0f, 1.0f, 0.0f, 
1.0f, 1.0f, 0.0f, 
1.0f, 1.0f, 0.0f, 
1.0f, 0.0f, 0.0f, 
1.0f, 0.0f, 0.0f, 
0.0f, 0.0f, 0.0f
};
const WireMesh WireMeshes::BOX_VERTICES = { BOX_VERTICES_points, sizeof(BOX_VERTICES_points) };