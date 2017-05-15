#include "world.h"

World::World()
{
    camera_position.x = 0;
    camera_position.y = 0;
    camera_position.z = 0;
    camera_up.x = 0;
    camera_up.y = 1;
    camera_up.z = 0;

    Teapot a;
    a.position.x = -1;
    a.position.y =  0;
    a.position.z = -3;
    a.spin = 0;

    Teapot b;
    b.position.x = -1;
    b.position.y =  1;
    b.position.z = -3;
    b.spin = 0;

    teapots.push_back(a);
    teapots.push_back(b);
}

World World::step(Input& input) const
{
    World new_world = *this;
    new_world.camera_position += input.movement;
    return new_world;
}
