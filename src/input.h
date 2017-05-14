#ifndef __INPUT_H
#define __INPUT_H

struct RawInput
{
    bool key_w, key_s, key_a, key_d;
};

struct Input 
{
    vec3 movement;

    Input(const RawInput& raw) 
    {
        if (raw.key_w) movement[2] = -1;
        if (raw.key_s) movement[2] =  1;
        if (raw.key_a) movement[0] = -1;
        if (raw.key_d) movement[0] =  1;
        vec3_norm(movement, movement);
    }
};

#endif//__INPUT_H