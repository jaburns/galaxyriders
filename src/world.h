#pragma once

#include <vector>
#include "deps/linmath.h"
#include "input.h"

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

    World();
    World step(Input& input) const;
};