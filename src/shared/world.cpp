#include "world.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <cmath>
#include "config.hpp"
#include "serialization.hpp"

const BakedLevel World::BAKED_LEVEL = BakedLevel::from_level(Level::from_data({
1.0f,39.0f,0.0f,383.5131f,-106.7458f,0.0f,-113.6776f,-105.6847f,0.0f,-113.6746f,60.29282f,0.0f,-96.52872f,32.61086f,0.0f,-50.92356f,45.47921f,0.0f,-18.6964f,45.29268f,50.0f,-25.01754f,-11.05426f,50.0f,-17.72211f,-17.35335f,0.0f,-7.363365f,-17.46777f,0.0f,42.68919f,-17.48897f,0.0f,42.70433f,-35.73095f,0.0f,51.56719f,-35.63853f,0.0f,51.57977f,-17.30403f,0.0f,72.65042f,-17.24803f,50.0f,84.98267f,-17.37347f,50.0f,104.8119f,-23.64916f,0.0f,119.17f,-24.08519f,0.0f,140.9189f,-24.10562f,0.0f,140.8984f,-32.22205f,0.0f,158.9607f,-32.22999f,0.0f,158.9369f,-24.02389f,0.0f,169.9889f,-24.09263f,0.0f,169.8818f,-32.26975f,0.0f,187.3026f,-32.27576f,0.0f,187.3126f,-24.13411f,0.0f,198.1484f,-24.13497f,0.0f,198.1618f,-32.24654f,0.0f,214.8907f,-32.1848f,0.0f,214.8486f,-24.13108f,0.0f,249.9169f,-24.10995f,50.0f,260.3928f,-24.31205f,50.0f,268.3945f,-22.60493f,0.0f,280.6699f,-6.108091f,0.0f,290.0646f,8.905025f,0.0f,306.7936f,9.556507f,50.0f,318.8844f,-10.8901f,50.0f,336.2576f,-6.70936f,0.0f,342.5138f,24.31914f,0.0f,383.6403f,245.7591f
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
    World world = next;
    world.player.position = glm::mix(player.position, next.player.position, t);
    return world;
}

static const float DT = Config::MILLIS_PER_TICK / 1000.0f;
static const float GRAVITY = 30.0f * DT * DT;
static const float RADIUS = 0.5f;
static const float WALK_ACCEL = 30.0f * DT * DT;
static const float PUMP_ACCEL = 80.0f * DT * DT;
static const float MAX_RUN_SPEED = 35.0f * DT;
static const float TURN_AROUND_MULTIPLIER = 3.0f;
static const float JUMP_SPEED = 10.0f * DT;
static const int LATE_JUMP_FRAMES = 5;

World World::step(const SharedInputState& old_input, const SharedInputState& new_input) const
{
    World next = *this;

    next.frame_counter++;

    // ----- Horizontal motion -----

    float walk_accel =
        new_input.left ? -WALK_ACCEL :
        new_input.right ? WALK_ACCEL : 0.0f;

    if (walk_accel * next.player.velocity.x < -1e-9f) {
        walk_accel *= TURN_AROUND_MULTIPLIER;
    } else if (fabs(next.player.velocity.x) > MAX_RUN_SPEED) {
        walk_accel = 0.0f;
    }

    next.player.velocity.x += walk_accel;

    // ----- Vertical motion -----

    next.player.velocity.y -= GRAVITY;

    if (!old_input.down && new_input.down) {
        if (next.player.velocity.y > 0.0f) next.player.velocity.y = 0.0f;
        next.player.grounded = 0;
    }

    if (new_input.down) {
        next.player.velocity.y -= PUMP_ACCEL;
    }

    if (!old_input.up && new_input.up && next.player.grounded > 0) {
        next.player.grounded = 0;
        next.player.velocity.y += JUMP_SPEED;
    }

    // ----- Collision detection and resolution -----

    const auto collision = BAKED_LEVEL.move_and_collide_circle(player.position, next.player.velocity , RADIUS, 0.0f);
    next.player.position = collision.position;
    next.player.velocity = collision.velocity;

    if (collision.collided) {
        next.player.ground_normal = collision.normal;
        if (collision.normal.y > 0.5f) {
            next.player.grounded = LATE_JUMP_FRAMES;
        }
    } else if (next.player.grounded > 0) {
        next.player.grounded--;
    }

    return next;
}
