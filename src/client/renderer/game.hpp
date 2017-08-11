#pragma once

#include "../../shared/lang_utils.hpp"
#include "../state.hpp"
#include "skybox.hpp"
#include "player.hpp"
#include "level.hpp"
#include "wire.hpp"

class GameRenderer : public NoCopy
{
    const SkyboxRenderer m_skybox_renderer;
    const LevelRenderer m_level_renderer;
    const PlayerRenderer m_player_renderer;
    const WireRenderer m_mouse_renderer;

public:
    GameRenderer();

    void render(const ClientState& state);
};