#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/quaternion.hpp>
#include <math.h>
#include "serialization.hpp"

World::World()
{
    camera_position = { 0.0f, 0.0f, 3.0f };
    camera_up = { 0.0f, 1.0f, 0.0f };
    camera_look = { 0.0f, 0.0f, -1.0f };
    frame_counter = 0;
}

template<typename T>
static void handle_serialization(World& world, T& buffer)
{
    SDBuffer::vec3(buffer, world.camera_position);
    SDBuffer::vec3(buffer, world.camera_up);
    SDBuffer::vec3(buffer, world.camera_look);
    buffer.val32(world.frame_counter);
}

World World::lerp_to(const World& next, float t) const
{
    World world;
    world.camera_position = glm::mix(world.camera_position, next.camera_position, t);
    world.camera_up = glm::mix(world.camera_up, next.camera_up, t);
    world.camera_look = glm::mix(world.camera_look, next.camera_look, t);
    world.frame_counter = next.frame_counter;
    return world;
}

World World::step(const InputState& input) const
{
    World world = *this;

    world.camera_position += 2.0f * input.movement;
    world.camera_look = input.look_dir;

    if (! input.clicking) {
        world.frame_counter++;
    }

    return world;
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