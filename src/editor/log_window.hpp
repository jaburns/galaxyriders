#pragma once

#include <imgui.h>
#include "window_state.hpp"

class LogWindow
{
    ImGuiTextBuffer m_buffer;
    ImGuiTextFilter m_filter;
    ImVector<int> m_line_offsets;
    int m_scroll_to_bottom;

public:
    WindowState window_state;

    void update();
};
