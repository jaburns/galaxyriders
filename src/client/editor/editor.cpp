#include "editor.hpp"

#include <imgui.h>
#include "../../shared/world.hpp"
#include "../../shared/debug.hpp"

EditorState Editor::update()
{
    ImGui::ShowDemoWindow();

    Debug::log("Wireframe? ", m_state.wireframe);

    ImGui::Begin("Debug Panel");
    ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
    ImGui::Checkbox("Wireframe", &m_state.wireframe);
    ImGui::End();

    return m_state;
}