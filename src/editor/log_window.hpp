#pragma once

#include <imgui.h>

struct WindowState
{
    bool open;
    bool should_reset;
};

class LogWindow
{
    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets;
    int scrollToBottom;

public:
    WindowState window_state;

    void update();
};
