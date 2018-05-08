#include "editor.hpp"

#include <imgui.h>
#include "../../shared/world.hpp"

EditorState Editor::update()
{
    ImGui::ShowDemoWindow();

    ImGui::Begin("Debug Panel");
    ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
    ImGui::End();

    return {};
}