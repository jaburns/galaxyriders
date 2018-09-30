#include "level_editor_window.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <fstream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <nlohmann/json.hpp>

#include "../client/core.hpp"
#include "../shared/world_state.hpp"

static constexpr float EDITMODE_CAMERA_SLIDE = 0.02f;
static constexpr float EDITMODE_CAMERA_ZOOM  = 1.10f;

void LevelEditorWindow::step_edit_mode(EditorState& editor_state, ClientState& client_state, const InputState& input, const InputState& last_input, const CoreView& core_view)
{
    auto& polys = LoadedLevel::get().polys;

    const auto mouse_pos = core_view.get_mouse_world_pos(client_state.camera_pos);
    float scroll_delta = input.mouse_scroll.y - last_input.mouse_scroll.y;

    // Update the currently selected level poly handle state.
    if (editor_state.selected_level_handle_state == EditorState::SelectedHandleState::Selected)
    {
        if (!input.mouse_click)
            editor_state.selected_level_handle_state = EditorState::SelectedHandleState::Hovered;

        polys[editor_state.selected_level_handle.poly]
            .handles[editor_state.selected_level_handle.handle]
            .point = mouse_pos;

        LoadedLevel::bake();
    }
    else if (!input.mouse_click)
    {
        editor_state.selected_level_handle_state = EditorState::SelectedHandleState::Not;

        const float min_dist2 = 1e0f;

        for (int i = 0; i < polys.size(); ++i)
        for (int j = 0; j < polys[i].handles.size(); ++j) 
        {
            const float new_d2 = glm::distance2(mouse_pos, polys[i].handles[j].point);

            if (new_d2 < min_dist2) {
                editor_state.selected_level_handle.poly = i;
                editor_state.selected_level_handle.handle = j;
                editor_state.selected_level_handle_state = EditorState::SelectedHandleState::Hovered;
            }
        }
    }

    if (editor_state.selected_level_handle_state == EditorState::SelectedHandleState::Hovered)
    {
        auto& handles = polys[editor_state.selected_level_handle.poly].handles;
        auto& handle = handles[editor_state.selected_level_handle.handle];

        const bool was_curve = handle.is_curve;
        if (scroll_delta < 0) handle.is_curve = false;
        else if (scroll_delta > 0) handle.is_curve = true;
        if (handle.is_curve != was_curve) LoadedLevel::bake();

        if (input.right_click)
        {
            editor_state.selected_level_handle_state = EditorState::SelectedHandleState::Not;
            handles.erase(handles.begin() + editor_state.selected_level_handle.handle);
            LoadedLevel::bake();
        }

        if (input.mouse_click)
        {
            if (input.pressing_shift)
            {
                handles.insert(handles.begin() + editor_state.selected_level_handle.handle, handle);
                editor_state.selected_level_handle.handle++;
            }

            editor_state.selected_level_handle_state = EditorState::SelectedHandleState::Selected;
        }
    }

    // Update the camera
    {
        const auto screen_mouse_pos = core_view.get_mouse_world_pos(glm::vec3(0, 0, client_state.camera_pos.z));

        if (editor_state.dragging_camera)
        {
            client_state.camera_pos = glm::vec3(editor_state.drag_camera_origin + (editor_state.drag_mouse_origin - screen_mouse_pos), client_state.camera_pos.z);

            if (!input.mouse_click)
                editor_state.dragging_camera = false;
        }
        else if (editor_state.selected_level_handle_state == EditorState::SelectedHandleState::Not)
        {
            if (scroll_delta > 0)
                client_state.camera_pos.z /= EDITMODE_CAMERA_ZOOM;
            else if (scroll_delta < 0)
                client_state.camera_pos.z *= EDITMODE_CAMERA_ZOOM;

            if (input.mouse_click)
            {
                editor_state.dragging_camera = true;
                editor_state.drag_mouse_origin = screen_mouse_pos;
                editor_state.drag_camera_origin = client_state.camera_pos;
            }
        }
    }
}

static void save_level_changes()
{
    std::ofstream file("res/levels/level.json");
    nlohmann::json contents;

    contents["level"] = LoadedLevel::get().serialize();

    file << std::setw(4) << contents;
    file.close();
}

static void save_physics_changes()
{
    std::ofstream file("res/physics.json");
    nlohmann::json contents;

    contents["gravity"] = Physics::GRAVITY;
    contents["jumpSpeed"] = Physics::JUMP_SPEED;
    contents["maxRunSpeed"] = Physics::MAX_RUN_SPEED;
    contents["pumpAccel"] = Physics::PUMP_ACCEL;
    contents["walkAccel"] = Physics::WALK_ACCEL;
    contents["turnaroundMult"] = Physics::TURN_AROUND_MULTIPLIER;

    file << std::setw(4) << contents;
    file.close();
}

void LevelEditorWindow::update(EditorState& editor_state, ClientState& client_state, const InputState& input_state, const InputState& last_input, const CoreView& core_view)
{
    if (!window_state.prepare_window()) return;

    auto& level = LoadedLevel::get();

    ImGui::Begin("Level Editor", &window_state.open);
        ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
        ImGui::SliderFloat("Jump Speed", &Physics::JUMP_SPEED, 0.0f, 1.0f);
        ImGui::SliderFloat("Max Run Speed", &Physics::MAX_RUN_SPEED, 0.0f, 1.0f);
        ImGui::SliderFloat("Pump Accel", &Physics::PUMP_ACCEL, 0.0f, 0.1f);
        ImGui::SliderFloat("Walk Accel", &Physics::WALK_ACCEL, 0.0f, 0.1f);
        ImGui::SliderFloat("Turnaround Mult", &Physics::TURN_AROUND_MULTIPLIER, 0.0f, 5.0f);

        if (ImGui::SliderFloat("Level Curve Quality", &level.curve_quality, 0.0f, 1.0f))
            LoadedLevel::bake();

        ImGui::Checkbox("Edit Mode", &editor_state.paused);
        ImGui::SameLine();
        ImGui::Checkbox("Wireframe", &editor_state.wireframe);

        ImGui::SameLine();
        if (ImGui::Button("Save Level Changes"))
            save_level_changes();

        ImGui::SameLine();
        if (ImGui::Button("Save Physics Changes"))
            save_physics_changes();

        ImGui::SameLine();
        if (ImGui::Button("Step Frame"))
            editor_state.single_frame_step = true;
    ImGui::End();

    if (editor_state.paused && !ImGui::GetIO().WantCaptureMouse)
        step_edit_mode(editor_state, client_state, input_state, last_input, core_view);
}
