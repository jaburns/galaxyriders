#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/quaternion.hpp>
#include <math.h>
#include "serialization.hpp"

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;
static const int MAX_POTS = 600;

void World::reset()
{
    camera_position = { 0.0f, 0.0f, 0.0f };
    camera_up = { 0.0f, 1.0f, 0.0f };
    camera_look = { 0.0f, 0.0f, -1.0f };

    Teapot ntp;
    ntp.transform.position.z = -3.0f;
    ntp.transform.scale = glm::vec3(0.01f);
    teapots.push_back(ntp);
}

World::World()
{
    reset();
}

std::vector<unsigned char> World::serialize()
{
    SerializationBuffer buf;
    buf.write_vec3(camera_position);
    return buf.buffer;
}

World::World(const unsigned char *serialized, int serialized_length)
{
    reset();
    DeserializationBuffer buf(serialized, serialized_length);
    buf.read_vec3(camera_position);
}

World World::step(InputState& input) const
{
    World world = *this;

    world.frame_counter++;
    world.camera_position += 2.0f * input.movement;
    world.camera_look = input.look_dir;

    if (frame_counter == 1 || frame_counter == 45 || frame_counter == 105) {
        world.parent_pot_tilt = glm::angleAxis(glm::linearRand(0.0f, 3.14159f), glm::sphericalRand(1.0f));
    }

    if (frame_counter < 120) {
        float sin_frame = sin(frame_counter / 60.0f * 2.0f * 3.14159f);
        float scale = 0.01f + (0.002f + 0.006f * frame_counter / 120.0f) * sin_frame;
        world.teapots.front().transform.scale = glm::vec3(scale);
        world.teapots.front().transform.rotation = glm::mix(glm::quat(), world.parent_pot_tilt, 0.5f * (0.5f + 0.5f*sin_frame));
        return world;
    }
    else if (frame_counter == 120) {
        world.teapots.clear();
    }

    if (input.clicking) {
        if (world.time_factor > 0.01f) {
            world.time_factor /= TIME_SLOW_DOWN;
        } else {
            world.time_factor = 0.01f;
        }
    } else {
        if (world.time_factor < 1.0f) {
            world.time_factor *= TIME_SPEED_UP;
        } else { 
            world.time_factor = 1.0f;
        }
    }

    if (world.time_factor > 0.9f) {
        Teapot ntp;
        ntp.transform.position.z = -3.0f;
        ntp.transform.scale = glm::vec3(glm::linearRand(0.005f, 0.01f));
        ntp.velocity.position = glm::sphericalRand(0.1f);
        ntp.velocity.rotation = glm::rotate(glm::quat(), glm::linearRand(-0.2f, 0.2f), glm::sphericalRand(1.0f));
        world.teapots.push_back(ntp);
    }

    if (world.teapots.size() > MAX_POTS) {
        world.teapots.pop_front();
    }

    for (auto& tp : world.teapots) {
        tp.velocity.position.y -= world.time_factor * 0.001f;
        tp.transform.position += world.time_factor * tp.velocity.position;
        tp.transform.rotation *= glm::mix(glm::quat(), tp.velocity.rotation, world.time_factor);
    }

    return world;
}

