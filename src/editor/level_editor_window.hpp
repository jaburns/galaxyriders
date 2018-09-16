#pragma once

#include <cstdint>
#include <imgui.h>

#include "../client/core.hpp"
#include "../client/client_state.hpp"

#include "editor_state.hpp"
#include "window_state.hpp"

class LevelEditorWindow
{
    int32_t m_selected_poly = -1;
    int32_t m_selected_handle = -1;

    void step_edit_mode(EditorState& editor_state, ClientState& client_state, const InputState& input, const InputState& last_input, const CoreView& core_view);

public:
    WindowState window_state;

    void update(EditorState& editor_state, ClientState& client_state, const InputState& input_state, const InputState& last_input_state, const CoreView& core_view);
};
