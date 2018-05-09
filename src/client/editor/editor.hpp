#pragma once

#include <imgui.h>
#include "editor_state.hpp"

struct ImguiLogState
{
    bool open = false;
    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets;
    int scrollToBottom;
};

class Editor
{
    ImguiLogState m_log_state;
    EditorState m_state;

    void draw_corner_overlay();

public:
    Editor();
    EditorState update();
};