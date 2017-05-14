#include "world.h"

#include <iostream>

World::World()
{
    camera_position[0] = 0;
    camera_position[1] = 0;
    camera_position[2] = 0;
    camera_up[0] = 0;
    camera_up[1] = 1;
    camera_up[2] = 0;

    Teapot a;
    a.position[0] = -1;
    a.position[1] =  0;
    a.position[2] = -3;
    a.spin = 0;

    Teapot b;
    b.position[0] = -1;
    b.position[1] =  1;
    b.position[2] = -3;
    b.spin = 0;

    teapots.push_back(a);
    teapots.push_back(b);
}

World World::step(Input& input) const
{
    using namespace std;

    World new_world = *this;
    cout << input.movement[0] << "\t" << input.movement[1] << "\t" << input.movement[2] << endl;
    vec3_add(new_world.teapots[0].position, new_world.teapots[0].position, input.movement);
    return new_world;
}