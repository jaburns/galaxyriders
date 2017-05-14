#pragma once

#include "deps/linmath.h"

struct RawInput
{
    bool key_w = false;
    bool key_s = false;
    bool key_a = false; 
    bool key_d = false;
};

struct Input 
{
    vec3 movement = { 0, 0, 0 };

    Input(const RawInput& raw)
    {
        bool not_zero = false;
        if (raw.key_w) { movement[2] = -1; not_zero = true; }
        if (raw.key_s) { movement[2] =  1; not_zero = true; }
        if (raw.key_a) { movement[0] = -1; not_zero = true; }
        if (raw.key_d) { movement[0] =  1; not_zero = true; }

        if (not_zero) {
            vec3_norm(movement, movement);
            vec3_scale(movement, movement, .1);
        }
    }
};