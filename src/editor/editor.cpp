#include "editor.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "../shared/world_state.hpp"

Editor::Editor()
    : m_state(), m_log_window()
{
    {
        std::ifstream file("res/levels/level.json");
        nlohmann::json contents;
        file >> contents;
        LoadedLevel::set(Level(contents["level"].get<std::string>()));
    }
    {
        std::ifstream file("res/physics.json");
        nlohmann::json contents;
        file >> contents;

        Physics::GRAVITY = contents["gravity"].get<float>();
        Physics::JUMP_SPEED = contents["jumpSpeed"].get<float>();
        Physics::MAX_RUN_SPEED = contents["maxRunSpeed"].get<float>();
        Physics::PUMP_ACCEL = contents["pumpAccel"].get<float>();
        Physics::WALK_ACCEL = contents["walkAccel"].get<float>();
        Physics::TURN_AROUND_MULTIPLIER = contents["turnaroundMult"].get<float>();
    }
}

static void window_buttons(const std::string& name, WindowState& window_state)
{
    if (ImGui::Button(("Show " + name).c_str())) {
        window_state.open = true;
    }
    ImGui::SameLine();
    if (ImGui::Button(("Reset " + name).c_str())) {
        window_state.should_reset = true;
    }
}

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
        window_buttons("Log", m_log_window.window_state);
        window_buttons("Level Editor", m_level_editor_window.window_state);

        ImGui::Separator();
        if (ImGui::IsMousePosValid()) {
            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        } else {
            ImGui::Text("Mouse Position: <invalid>");
        }

    ImGui::End();
}

EditorState Editor::update(ClientState& client_state, const InputState& input_state, const CoreView& core_view)
{
//  ImGui::ShowDemoWindow();

    if (m_state.single_frame_step) {
        m_state.single_frame_step = false;
        m_state.paused = true;
    }

    draw_corner_overlay();

    m_log_window.update();
    m_level_editor_window.update(m_state, client_state, input_state, m_last_input, core_view);

    if (m_state.single_frame_step) {
        m_state.paused = false;
    }

    m_last_input = input_state;

    return m_state;
}
