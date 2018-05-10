#pragma once

#include <glm/mat4x4.hpp>
#include "../client_state.hpp"
#include "../../shared/lang_utils.hpp"
#include "sprite_renderer.hpp"
#include "wire_renderer.hpp"

class PlayerRenderer : public NoCopy
{
    const SpriteRenderer m_sprite_renderer;
    const WireRenderer m_wire_sphere_renderer;

public:
    PlayerRenderer();

    void draw_once(
        const glm::mat4x4& view, const glm::mat4x4& projection,
        const WorldState::Player& player, const ClientState::PlayerAnimation& anim
    ) const;
};
