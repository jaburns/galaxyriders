#include "level_editor_window.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../client/core.hpp"
#include "../shared/world_state.hpp"

static constexpr float EDITMODE_CAMERA_SLIDE = 0.05f;
static constexpr float EDITMODE_CAMERA_ZOOM  = 1.05f;

void LevelEditorWindow::step_edit_mode(EditorState& editor_state, ClientState& client_state, const InputState& input, const CoreView& core_view)
{
    if (input.player.right) client_state.camera_pos.x += EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
    if (input.player.left)  client_state.camera_pos.x -= EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
    if (input.player.up)    client_state.camera_pos.y += EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
    if (input.player.down)  client_state.camera_pos.y -= EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;

    auto& polys = LoadedLevel::get().polys;

    const auto mouse_pos = core_view.get_mouse_world_pos(client_state.camera_pos);

    editor_state.selected_level_handle_state = EditorState::SelectedHandleState::Not;
    editor_state.selected_level_handle.poly = -1;

    const float min_dist2 = 1e0f;

    for (int i = 0; i < polys.size(); ++i)
    for (int j = 0; j < polys[i].handles.size(); ++j) 
    {
        const float new_d2 = glm::distance2(mouse_pos, polys[i].handles[j].point);

        if (new_d2 < min_dist2) {
            editor_state.selected_level_handle.poly = i;
            editor_state.selected_level_handle.handle = j;
        }
    }

    editor_state.selected_level_handle_state = editor_state.selected_level_handle.poly < 0
        ? EditorState::SelectedHandleState::Not
        : input.mouse_click
        ? EditorState::SelectedHandleState::Selected
        : EditorState::SelectedHandleState::Hovered;

    if (editor_state.selected_level_handle_state == EditorState::SelectedHandleState::Selected)
    {
        polys[editor_state.selected_level_handle.poly]
            .handles[editor_state.selected_level_handle.handle]
            .point = mouse_pos;

        LoadedLevel::bake();
    }
}

void LevelEditorWindow::update(EditorState& editor_state, ClientState& client_state, const InputState& input_state, const CoreView& core_view)
{
    if (!window_state.prepare_window()) return;

    ImGui::Begin("Level Editor", &window_state.open);
        ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
        ImGui::Checkbox("Edit Mode", &editor_state.paused);
        ImGui::SameLine();
        ImGui::Checkbox("Wireframe", &editor_state.wireframe);

        if (ImGui::Button("Step Frame")) {
            editor_state.single_frame_step = true;
        }
    ImGui::End();

    if (editor_state.paused && !ImGui::GetIO().WantCaptureMouse) {
        step_edit_mode(editor_state, client_state, input_state, core_view);
    }
}
