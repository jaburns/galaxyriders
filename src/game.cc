#include "game.h"
#include "deps\linmath.h"

World Game::init()
{
    World w;
    w.camera_position[0] = 0;
    w.camera_position[1] = 0;
    w.camera_position[2] = 0;
    w.camera_up[0] = 0;
    w.camera_up[1] = 1;
    w.camera_up[2] = 0;

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

    w.teapots.push_back(a);
    w.teapots.push_back(b);

    return w;
}

World Game::step(const World& world, const Input& input) 
{
    World new_world = world;
    vec3_add(new_world.camera_position, world.camera_position, input.movement);
    return new_world;
}
