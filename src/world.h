#ifndef __WORLD_H
#define __WORLD_H

#include "deps/linmath.h"
#include <vector>

struct Teapot
{
    vec3 position;
    float spin;
};

struct World 
{
    vec3 camera_position;
    vec3 camera_up;

    std::vector<Teapot> teapots;
};


#endif//__WORLD_H
