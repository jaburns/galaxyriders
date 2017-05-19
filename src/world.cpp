#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <math.h>

World::World()
{
    camera_position = { 0.0f, 0.0f, 0.0f };
    camera_up = { 0.0f, 1.0f, 0.0f };
    camera_look = { 0.0f, 0.0f, -1.0f };
}

World World::step(InputState& input) const
{
    World world = *this;

    world.camera_position += input.movement;
    world.camera_look = input.look_dir;

    Teapot ntp;
    ntp.transform.position.z = -3.0f;
    ntp.transform.scale = glm::vec3(glm::linearRand(0.005f, 0.01f));
    ntp.velocity.position = glm::sphericalRand(0.1f);
    ntp.velocity.rotation = glm::rotate(glm::quat(), glm::linearRand(-0.2f, 0.2f), glm::sphericalRand(1.0f));
    world.teapots.push_back(ntp);

    for (auto& tp : world.teapots) {
        tp.velocity.position.y -= 0.001f;
        tp.transform.position += tp.velocity.position;
        tp.transform.rotation *= tp.velocity.rotation;
    }

    return world;
}
