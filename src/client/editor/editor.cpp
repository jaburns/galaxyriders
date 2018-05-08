#include "editor.hpp"

#include <imgui.h>
#include "../../shared/world.hpp"

Editor::Editor()
    : m_state() 
{}

EditorState Editor::update()
{
    ImGui::ShowDemoWindow();

    ImGui::Begin("Debug Panel");
    ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
    ImGui::Checkbox("Wireframe", &m_state.wireframe);
    ImGui::End();

    return m_state;
}