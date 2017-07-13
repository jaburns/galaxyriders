#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <math.h>
#include "serialization.hpp"

const BakedLevel World::BAKED_LEVEL = BakedLevel::from_level(Level::from_data({
1,39,0,2513391,-699569,0,-744998,-692615,0,-744978,395135,0,-632611,213719,0,-333733,298053,0,-122529,296830,50,-163955,-72445,50,-116144,-113727,0,-48257,-114477,0,279768,-114616,0,279867,-234166,0,337951,-233561,0,338033,-113404,0,476122,-113037,50,556942,-113859,50,686895,-154987,0,780993,-157845,0,923526,-157979,0,923392,-211170,0,1041765,-211222,0,1041609,-157443,0,1114039,-157893,0,1113337,-211483,0,1227506,-211522,0,1227572,-158165,0,1298585,-158171,0,1298673,-211331,0,1408308,-210926,0,1408032,-158145,0,1637855,-158007,50,1706510,-159331,50,1758950,-148144,0,1834564,-37471,0,1900967,58360,0,2010603,62630,50,2089841,-71369,19,2203698,-43970,0,2244698,159378,0,2514225,1610607
}));

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
    buf.write_val32(player.velocity.x.to_raw_int());
    buf.write_val32(player.velocity.y.to_raw_int());
    buf.write_val32(player.position.x.to_raw_int());
    buf.write_val32(player.position.y.to_raw_int());

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
    player.velocity.x = fixed32::from_raw_int(buf.read_val32<int32_t>());
    player.velocity.y = fixed32::from_raw_int(buf.read_val32<int32_t>());
    player.position.x = fixed32::from_raw_int(buf.read_val32<int32_t>());
    player.position.y = fixed32::from_raw_int(buf.read_val32<int32_t>());
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

static const fixed32 MOVEMENT_SPEED = fixed32(5);
static const fixed32 GRAVITY = fixed32::from_fraction(-1, 100);
static const fixed32 RADIUS = fixed32::from_fraction(1, 10);

World World::step(const InputState& input) const
{
    World world = *this;

    world.camera_position += MOVEMENT_SPEED * input.movement;
    world.camera_look = input.look_dir;

    if (input.clicking) {
        world.player.velocity.y += GRAVITY;
        world.player.position += world.player.velocity;

        const auto collision = BAKED_LEVEL.collide_circle(player.position, world.player.position, RADIUS);
        if (collision.collided) {
            world.player.position = collision.restore;
            world.player.velocity = fixed32::reflect(world.player.velocity, collision.normal, fixed32::ZERO, fixed32::ONE);
        }

        world.frame_counter++;
    }

    return world;
}
