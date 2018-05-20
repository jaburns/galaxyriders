#include "window_state.hpp"
#include <imgui.h>

bool WindowState::prepare_window()
{
    if (! open) return false;

    if (should_reset) {
        ImGui::SetNextWindowSize(ImVec2(400.0f, 300.0f));
        ImGui::SetNextWindowPosCenter();
        should_reset = false;
    }

    return true;
}