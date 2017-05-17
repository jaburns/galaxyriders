#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <math.h>

World::World()
{
    camera_position = { 0.0f, 0.0f, 0.0f };
    camera_up = { 0.0f, 1.0f, 0.0f };

    Teapot a;
    a.position = { -1.0f, 0.0f, -3.0f };
    a.spin = 0.0f;

    Teapot b;
    b.position = { 1.0, 0.0f, -3.0f };
    b.spin = glm::pi<float>();

    teapots.push_back(a);
    teapots.push_back(b);
}

World World::step(InputState& input) const
{
    World world = *this;

    world.camera_position += input.movement;
    world.camera_look = input.look_dir;

    for (auto& tp : world.teapots) {
        tp.spin += 0.02f;
        tp.position.y = sin(tp.spin);
    }

    return world;
}
