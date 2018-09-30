#pragma once
#include "../../shared/global.hpp"

#include "../../shared/lang_utils.hpp"
#include "../../editor/editor_state.hpp"
#include "../client_state.hpp"
#include "player_renderer.hpp"
#include "loaded_level_renderer.hpp"
#include "wire_renderer.hpp"

class GameRenderer : public NoCopy
{
    const PlayerRenderer m_player_renderer;
    LoadedLevelRenderer m_level_renderer;

public:
    GameRenderer();

    void render(const ClientState& state, const CoreView& core_view, const EditorState& editor_state);
};
