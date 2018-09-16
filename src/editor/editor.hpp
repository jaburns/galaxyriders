#pragma once

#include "../client/core.hpp"
#include "../client/client_state.hpp"

#include "editor_state.hpp"
#include "log_window.hpp"
#include "level_editor_window.hpp"

class Editor
{
    EditorState m_state;
    LogWindow m_log_window;
    LevelEditorWindow m_level_editor_window;
    InputState m_last_input;

    void draw_corner_overlay();

public:
    Editor();
    EditorState update(ClientState& client_state, const InputState& input_state, const CoreView& core_view);
};