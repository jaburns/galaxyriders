#pragma once

#include <glm/mat4x4.hpp>
#include "../state.hpp"
#include "../../shared/lang_utils.hpp"
#include "sprite.hpp"
#include "wire.hpp"

class PlayerRenderer : public NoCopy
{
    const SpriteRenderer m_sprite_renderer;
    const WireRenderer m_wire_sphere_renderer;

public:
    PlayerRenderer();

    void draw_once(
        const glm::mat4x4& view, const glm::mat4x4& projection,
        const World::Player& player, const ClientState::PlayerAnimation& anim
    ) const;
};
