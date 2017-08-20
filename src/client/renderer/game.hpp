#pragma once

#include "../../shared/lang_utils.hpp"
#include "../state.hpp"
#include "player.hpp"
#include "level.hpp"
#include "wire.hpp"

class GameRenderer : public NoCopy
{
    const PlayerRenderer m_player_renderer;
    LoadedLevelRenderer m_level_renderer;

public:
    GameRenderer();

    void render(const ClientState& state);
};
