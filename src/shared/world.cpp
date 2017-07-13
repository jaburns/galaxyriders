#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <math.h>
#include "serialization.hpp"

World::World()
{
    camera_position = { fixed32::ZERO, fixed32::ZERO, fixed32(3) };
    camera_look = { fixed32::ZERO, fixed32::ZERO, fixed32::MINUS_ONE };
    frame_counter = 0;
}

std::vector<uint8_t> World::serialize() const
{
    SerializationBuffer buf;

    buf.write_val32(camera_position.x.to_raw_int());
    buf.write_val32(camera_position.y.to_raw_int());
    buf.write_val32(camera_position.z.to_raw_int());
    buf.write_val32(camera_look.x.to_raw_int());
    buf.write_val32(camera_look.y.to_raw_int());
    buf.write_val32(camera_look.z.to_raw_int());
    buf.write_val32(frame_counter);

    return buf.buffer;
}

World::World(const uint8_t *serialized, int serialized_length)
{
    DeserializationBuffer buf(serialized, serialized_length);

    camera_position.x = fixed32::from_raw_int(buf.read_val32<int32_t>());
    camera_position.y = fixed32::from_raw_int(buf.read_val32<int32_t>());
    camera_position.z = fixed32::from_raw_int(buf.read_val32<int32_t>());
    camera_look.x = fixed32::from_raw_int(buf.read_val32<int32_t>());
    camera_look.y = fixed32::from_raw_int(buf.read_val32<int32_t>());
    camera_look.z = fixed32::from_raw_int(buf.read_val32<int32_t>());
    frame_counter = buf.read_val32<int32_t>();
}

World World::lerp_to(const World& next, float t) const
{
    auto mix = [&t](const auto& a, const auto& b) {
        return a + fixed32::from_float(t) * (b - a);
    };

    World world = *this;
    world.camera_position = mix(world.camera_position, next.camera_position);
    world.camera_look = mix(world.camera_look, next.camera_look);
    world.frame_counter = next.frame_counter;
    return world;
}

World World::step(const InputState& input) const
{
    const fixed32 MOVEMENT_SPEED = fixed32(5);
    World world = *this;

    world.camera_position += MOVEMENT_SPEED * input.movement;
    world.camera_look = input.look_dir;

    if (! input.clicking) {
        world.frame_counter++;
    }

    return world;
}
