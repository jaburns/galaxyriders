#pragma once

#include "editor_state.hpp"
#include "log_window.hpp"

class Editor
{
    LogWindow m_log_window;
    EditorState m_state;

    void draw_corner_overlay();

public:
    Editor();
    EditorState update();
};