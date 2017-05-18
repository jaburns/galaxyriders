#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <math.h>
#include <cstdlib>

World::World()
{
    camera_position = { 0.0f, 0.0f, 0.0f };
    camera_up = { 0.0f, 1.0f, 0.0f };
    camera_look = { 0.0f, 0.0f, -1.0f };
}

static float rand_range(float lower, float upper)
{
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return lower + r * (upper - lower);
}

static glm::vec3 rand_normal()
{
    glm::vec3 v = { rand_range(-1,1), rand_range(-1,1), rand_range(-1,1) };
    return glm::normalize(v);
}

World World::step(InputState& input) const
{
    World world = *this;

    world.camera_position += input.movement;
    world.camera_look = input.look_dir;

    Teapot ntp;
    ntp.transform.position.z = -3.0f;
    ntp.transform.scale = glm::vec3(0.01f);
    ntp.velocity.position = 0.1f * rand_normal();
    ntp.velocity.rotation = glm::rotate(glm::quat(), rand_range(-0.2f, 0.2f), rand_normal());
    world.teapots.push_back(ntp);

    for (auto& tp : world.teapots) {
        tp.velocity.position.y -= 0.001f;
        tp.transform.position += tp.velocity.position;
        tp.transform.rotation *= tp.velocity.rotation;
    }

    return world;
}
