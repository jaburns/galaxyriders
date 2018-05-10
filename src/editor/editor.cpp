#include "editor.hpp"

#include "../shared/world_state.hpp"

Editor::Editor()
    : m_state(), m_log_window()
{}

void Editor::draw_corner_overlay()
{
    const float DISTANCE = 10.0f;
    ImVec2 window_pos = ImVec2(DISTANCE, DISTANCE); 
    ImVec2 window_pos_pivot = ImVec2(0.0f, 0.0f);
    ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

    ImGui::Begin("Corner Overlay", nullptr, flags);
        if (ImGui::Button("Show Log")) {
            m_log_window.window_state.open = true;
        }
        if (ImGui::Button("Reset Log")) {
            m_log_window.window_state.should_reset = true;
        }
        ImGui::Separator();
        if (ImGui::IsMousePosValid()) {
            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        } else {
            ImGui::Text("Mouse Position: <invalid>");
        }
    ImGui::End();
}
EditorState Editor::update()
{
//  ImGui::ShowDemoWindow();

    draw_corner_overlay();

    m_log_window.update();

    ImGui::Begin("Debug Panel");
    ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
    ImGui::Checkbox("Wireframe", &m_state.wireframe);
    ImGui::End();

    return m_state;
}