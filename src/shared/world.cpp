#include "world.hpp"

                #include "debug.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <cmath>
#include "config.hpp"
#include "serialization.hpp"

static constexpr float DT = Config::MILLIS_PER_TICK / 1000.0f;
static constexpr float GRAVITY = 30.0f * DT * DT;
static constexpr float RADIUS = 0.5f;
static constexpr float WALK_ACCEL = 30.0f * DT * DT;
static constexpr float PUMP_ACCEL = 80.0f * DT * DT;
static constexpr float MAX_RUN_SPEED = 35.0f * DT;
static constexpr float TURN_AROUND_MULTIPLIER = 3.0f;
static constexpr float JUMP_SPEED = 10.0f * DT;
static constexpr int LATE_JUMP_FRAMES = 5;

std::vector<uint8_t> World::serialize() const
{
    SerializationBuffer buf;

    buf.write_val32(frame_counter);
    buf.write_byte(players.size());

    for (const auto& p : players) {
        buf.write_val32(p.first);
        buf.write_val32(p.second.velocity.x);
        buf.write_val32(p.second.velocity.y);
        buf.write_val32(p.second.position.x);
        buf.write_val32(p.second.position.y);
        buf.write_val32(p.second.ground_normal.x);
        buf.write_val32(p.second.ground_normal.y);
        buf.write_byte(p.second.grounded);
        buf.write_byte(p.second.air_stomping ? 0xFF : 0x00);
    }

    return buf.buffer;
}

World::World(const uint8_t *serialized, int serialized_length)
{
    DeserializationBuffer buf(serialized, serialized_length);

    frame_counter = buf.read_val32<int32_t>();
    const auto num_players = buf.read_byte();

    players.clear();
    for (auto i = 0; i < num_players; ++i) {
        const auto id = buf.read_val32<int32_t>();
        players[id].velocity.x = buf.read_val32<float>();
        players[id].velocity.y = buf.read_val32<float>();
        players[id].position.x = buf.read_val32<float>();
        players[id].position.y = buf.read_val32<float>();
        players[id].ground_normal.x = buf.read_val32<float>();
        players[id].ground_normal.y = buf.read_val32<float>();
        players[id].grounded = buf.read_byte();
        players[id].air_stomping = buf.read_byte() != 0;
    }
}

static World::Player lerp_player(const World::Player& first, const World::Player& next, float t)
{
    World::Player result = next;
    result.position = glm::mix(first.position, next.position, t);
    return result;
}

World World::lerp_to(const World& next, float t) const
{
    auto world = next;

    for (auto& p : world.players) {
        if (players.find(p.first) == players.end()) continue;

        p.second.position = glm::mix(players.at(p.first).position, p.second.position, t);
    }

    return world;
}

static void update_player(World::Player& player, const PlayerInput& old_input, const PlayerInput& new_input)
{
    // ----- Horizontal motion -----

    auto walk_accel =
        new_input.left ? -WALK_ACCEL :
        new_input.right ? WALK_ACCEL : 0.0f;

    if (walk_accel * player.velocity.x < -1e-9f) {
        walk_accel *= TURN_AROUND_MULTIPLIER;
    } else if (fabs(player.velocity.x) > MAX_RUN_SPEED) {
        walk_accel = 0.0f;
    }

    player.velocity.x += walk_accel;

    // ----- Vertical motion -----

    player.velocity.y -= GRAVITY;

    if (!old_input.down && new_input.down) {
        if (player.velocity.y > 0.0f) player.velocity.y = 0.0f;
        player.grounded = 0;
    }

    if (new_input.down) {
        player.air_stomping = true;
        player.velocity.y -= PUMP_ACCEL;
    }

    if (!old_input.up && new_input.up && player.grounded > 0) {
        player.grounded = 0;
        player.velocity.y += JUMP_SPEED;
    }

    // ----- Collision detection and resolution -----

    const auto collision = LoadedLevel::get_baked().move_and_collide_circle(player.position, player.velocity , RADIUS, 0.0f);
    player.position = collision.position;
    player.velocity = collision.velocity;

    if (collision.collided) {
        player.ground_normal = collision.normal;
        if (collision.normal.y > 0.5f) {
            player.grounded = LATE_JUMP_FRAMES;
        }
        player.air_stomping = false;
    } else if (player.grounded > 0) {
        player.grounded--;
    }
}

void World::step(const PlayerInput& old_input, const PlayerInput& new_input)
{
    frame_counter++;

    for (auto& p : players) {
        update_player(p.second, old_input, new_input);
    }
}