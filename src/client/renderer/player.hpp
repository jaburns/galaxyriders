#pragma once

#define _USE_MATH_DEFINES

#include <memory>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../../shared/world.hpp"
#include "../state.hpp"
#include "sprite.hpp"
#include "wire.hpp"

class PlayerRenderer
{
    std::unique_ptr<SpriteRenderer> m_sprite_renderer;
    std::unique_ptr<WireRenderer> m_wire_sphere_renderer;

    PlayerRenderer(const PlayerRenderer&) =delete;
    PlayerRenderer& operator=(const PlayerRenderer&) =delete;

public:
    PlayerRenderer();

    void draw_once(
        const glm::mat4x4& view, const glm::mat4x4& projection,
        const World::Player& player, const ClientState::PlayerAnimation& anim
    );
};
