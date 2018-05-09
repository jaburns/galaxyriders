#pragma once

#include <imgui.h>
#include "editor_state.hpp"

struct ImguiLogState
{
    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets;
    int scrollToBottom;
};

class Editor
{
    ImguiLogState m_log_state;
    EditorState m_state;

public:
    Editor();
    EditorState update();
};