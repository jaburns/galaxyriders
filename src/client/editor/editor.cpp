#include "editor.hpp"

#include "../../shared/world.hpp"
#include "../../shared/logger.hpp"

Editor::Editor()
    : m_state(), m_log_state()
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
            m_log_state.open = true;
        }
        ImGui::Separator();
        if (ImGui::IsMousePosValid()) {
            ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        } else {
            ImGui::Text("Mouse Position: <invalid>");
        }
    ImGui::End();
}

static void update_log_window(ImguiLogState& log)
{
    if (!log.open) {
        return;
    }

    if (! Logger::instance().empty()) {
        log.buffer.appendf("%s", Logger::instance().flush().c_str());
        log.scrollToBottom = 2;
    }

    // if reset window position clicked
        ImGui::SetNextWindowSize(ImVec2(400.0f, 300.0f));
        ImGui::SetNextWindowPosCenter();
    // fi

    ImGui::Begin("Log", &log.open);
        if (ImGui::Button("Clear")) {
            log.buffer.clear();
            log.lineOffsets.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy")) {
            ImGui::LogToClipboard();
        }
        ImGui::SameLine();
        log.filter.Draw("Filter", -100.0f);
        ImGui::Separator();

        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
            if (log.filter.IsActive()) {
                const char* buf_begin = log.buffer.begin();
                const char* line = buf_begin;

                for (int line_no = 0; line != NULL; line_no++) {
                    const char* line_end = (line_no < log.lineOffsets.Size) ? buf_begin + log.lineOffsets[line_no] : NULL;

                    if (log.filter.PassFilter(line, line_end)) {
                        ImGui::TextUnformatted(line, line_end);
                    }

                    line = line_end && line_end[1] ? line_end + 1 : NULL;
                }
            } else {
                ImGui::TextUnformatted(log.buffer.begin());
            }

            if (log.scrollToBottom > 0) {
                log.scrollToBottom--;
                ImGui::SetScrollHere(1.0f);
            }
        ImGui::EndChild();
    ImGui::End();
}

EditorState Editor::update()
{
//  ImGui::ShowDemoWindow();

    draw_corner_overlay();
    update_log_window(m_log_state);

    ImGui::Begin("Debug Panel");
    ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
    ImGui::Checkbox("Wireframe", &m_state.wireframe);
    ImGui::End();

    return m_state;
}