#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <math.h>
#include "serialization.hpp"

const BakedLevel World::BAKED_LEVEL = BakedLevel::from_level(Level::from_data({
1,39,0,2513391,-699569,0,-744998,-692615,0,-744978,395135,0,-632611,213719,0,-333733,298053,0,-122529,296830,50,-163955,-72445,50,-116144,-113727,0,-48257,-114477,0,279768,-114616,0,279867,-234166,0,337951,-233561,0,338033,-113404,0,476122,-113037,50,556942,-113859,50,686895,-154987,0,780993,-157845,0,923526,-157979,0,923392,-211170,0,1041765,-211222,0,1041609,-157443,0,1114039,-157893,0,1113337,-211483,0,1227506,-211522,0,1227572,-158165,0,1298585,-158171,0,1298673,-211331,0,1408308,-210926,0,1408032,-158145,0,1637855,-158007,50,1706510,-159331,50,1758950,-148144,0,1834564,-37471,0,1900967,58360,0,2010603,62630,50,2089841,-71369,19,2203698,-43970,0,2244698,159378,0,2514225,1610607
}));

std::vector<uint8_t> World::serialize() const
{
    SerializationBuffer buf;

    buf.write_val32(frame_counter);
    buf.write_val32(player.velocity.x);
    buf.write_val32(player.velocity.y);
    buf.write_val32(player.position.x);
    buf.write_val32(player.position.y);

    return buf.buffer;
}

World::World(const uint8_t *serialized, int serialized_length)
{
    DeserializationBuffer buf(serialized, serialized_length);

    frame_counter = buf.read_val32<int32_t>();
    player.velocity.x = buf.read_val32<float>();
    player.velocity.y = buf.read_val32<float>();
    player.position.x = buf.read_val32<float>();
    player.position.y = buf.read_val32<float>();
}

World World::lerp_to(const World& next, float t) const
{
    World world = *this;
    world.player.position = glm::mix(world.player.position, next.player.position, t);
    world.frame_counter = next.frame_counter;
    return world;
}

static const float GRAVITY = -0.02f;
static const float RADIUS = 0.1f;

// TODO find a better place for these vector methods
static glm::vec2 rotate90(const glm::vec2& v)
{
    return { -v.y, v.x };
}

static glm::vec2 reflect(const glm::vec2& v, const glm::vec2& unit_normal, float normal_scale, float tangent_scale)
{
    const auto unit_tangent = rotate90(unit_normal);

    const auto norm_component = -normal_scale * glm::dot(v, unit_normal);
    const auto tang_component = tangent_scale * glm::dot(v, unit_tangent);

    return unit_normal * norm_component + unit_tangent * tang_component;
}

World World::step(const SharedInputState& input) const
{
    World world = *this;

    world.frame_counter++;

    world.player.velocity.y += GRAVITY;
    world.player.position += world.player.velocity;

    if (input.left)  world.player.velocity.x += GRAVITY;
    if (input.right) world.player.velocity.x -= GRAVITY;

    const auto collision = BAKED_LEVEL.collide_circle(player.position, world.player.position, RADIUS);
    if (collision.collided) {
        world.player.position = collision.restore;
        world.player.velocity = reflect(world.player.velocity, collision.normal, 0.0f, 1.0f);
    }

    return world;
}
