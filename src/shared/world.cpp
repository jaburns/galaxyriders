#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/quaternion.hpp>
#include <math.h>
#include "serialization.hpp"

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;
static const int MAX_POTS = 60;

World::World()
{
    camera_position = { 0.0f, 0.0f, 0.0f };
    camera_up = { 0.0f, 1.0f, 0.0f };
    camera_look = { 0.0f, 0.0f, -1.0f };

    Teapot ntp;
    ntp.transform.position.z = -3.0f;
    ntp.transform.scale = glm::vec3(0.01f);
    teapots.push_back(ntp);
}

template<typename T>
static void handle_serialization(World& world, T& buffer)
{
    SDBuffer::vec3(buffer, world.camera_position);
    SDBuffer::vec3(buffer, world.camera_up);
    SDBuffer::vec3(buffer, world.camera_look);
    SDBuffer::quat(buffer, world.parent_pot_tilt);
    buffer.val32(world.frame_counter);
    buffer.val32(world.time_factor);

    int pots = buffer.container_size(world.teapots);

    for (int i = 0; i < pots; ++i) {
        SDBuffer::vec3(buffer, world.teapots[i].transform.position);
        SDBuffer::quat(buffer, world.teapots[i].transform.rotation);
        SDBuffer::vec3(buffer, world.teapots[i].transform.scale);
        SDBuffer::vec3(buffer, world.teapots[i].velocity.position);
        SDBuffer::quat(buffer, world.teapots[i].velocity.rotation);
        SDBuffer::vec3(buffer, world.teapots[i].velocity.scale);
    }
}

std::vector<unsigned char> World::serialize() const
{
    SerializationBuffer buf;
    handle_serialization(*const_cast<World*>(this), buf);
    return buf.buffer;
}

World::World(const unsigned char *serialized, int serialized_length)
{
    DeserializationBuffer buf(serialized, serialized_length);
    handle_serialization(*this, buf);
}

World World::lerp_to(const World& next, float t) const
{
    World world = *this;
    world.camera_position = glm::mix(world.camera_position, next.camera_position, t);
    world.camera_up = glm::mix(world.camera_up, next.camera_up, t);
    world.camera_look = glm::mix(world.camera_look, next.camera_look, t);
    world.parent_pot_tilt = glm::slerp(world.parent_pot_tilt, next.parent_pot_tilt, t);
    world.time_factor = glm::mix(world.time_factor, next.time_factor, t);

    for (auto i = 0; i < world.teapots.size() && i < next.teapots.size(); ++i) {
        world.teapots[i].transform.position = glm::mix(world.teapots[i].transform.position, next.teapots[i].transform.position, t);
        world.teapots[i].transform.rotation = glm::slerp(world.teapots[i].transform.rotation, next.teapots[i].transform.rotation, t);
        world.teapots[i].transform.scale = glm::mix(world.teapots[i].transform.scale, next.teapots[i].transform.scale, t);
        world.teapots[i].velocity.position = glm::mix(world.teapots[i].velocity.position, next.teapots[i].velocity.position, t);
        world.teapots[i].velocity.rotation = glm::slerp(world.teapots[i].velocity.rotation, next.teapots[i].velocity.rotation, t);
        world.teapots[i].velocity.scale = glm::mix(world.teapots[i].velocity.scale, next.teapots[i].velocity.scale, t);
    }
    return world;
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
