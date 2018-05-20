#include "level_editor_window.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../client/core.hpp"
#include "../shared/world_state.hpp"

static constexpr float EDITMODE_CAMERA_SLIDE = 0.05f;
static constexpr float EDITMODE_CAMERA_ZOOM  = 1.05f;

void LevelEditorWindow::step_edit_mode(ClientState& client_state, const InputState& input, const CoreView& core_view)
{
    if (input.player.right)   client_state.camera_pos.x += EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
    if (input.player.left)    client_state.camera_pos.x -= EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
    if (input.player.up)      client_state.camera_pos.y += EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
    if (input.player.down)    client_state.camera_pos.y -= EDITMODE_CAMERA_SLIDE * client_state.camera_pos.z;
//  if (input.editmode_zoom_out) state.camera_pos.z *= EDITMODE_CAMERA_ZOOM;
//  if (input.editmode_zoom_in)  state.camera_pos.z /= EDITMODE_CAMERA_ZOOM;

    if (input.mouse_click) {
        const auto& mouse_pos = core_view.get_mouse_world_pos(client_state.camera_pos);

        m_selected_handle = 0;
        m_selected_poly = 0;
        float min_dist2 = 1e20f;
        for (int32_t i = 0; i < LoadedLevel::get().polys.size(); ++i) {
            for (int32_t j = 0; j < LoadedLevel::get().polys[i].handles.size(); ++j) {
                const auto& h = LoadedLevel::get().polys[i].handles[j];
                const auto new_d2 = glm::distance2(mouse_pos, h.point);
                if (new_d2 < min_dist2) {
                    m_selected_handle = i;
                    m_selected_poly = j;
                    min_dist2 = new_d2;
                }
            }
        }

        LoadedLevel::get().polys[m_selected_handle].handles[m_selected_poly].point = mouse_pos;
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
    ImGui::End();

    if (editor_state.paused && !ImGui::GetIO().WantCaptureMouse) {
        step_edit_mode(client_state, input_state, core_view);
    }
}
