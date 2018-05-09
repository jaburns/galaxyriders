#include "editor.hpp"

#include "../../shared/world.hpp"
#include "../../shared/logger.hpp"

Editor::Editor()
    : m_state(), m_log_state()
{}

static void update_log_window(ImguiLogState& log)
{
    if (! Logger::instance().empty()) {
        log.buffer.appendf("%s", Logger::instance().flush().c_str());
        log.scrollToBottom = 2;
    }

    ImGui::Begin("Log");

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

    update_log_window(m_log_state);

    ImGui::Begin("Debug Panel");
    ImGui::SliderFloat("Gravity", &Physics::GRAVITY, 0.0f, 0.1f);
    ImGui::Checkbox("Wireframe", &m_state.wireframe);
    ImGui::End();

    return m_state;
}