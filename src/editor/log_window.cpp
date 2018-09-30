#include "log_window.hpp"

void LogWindow::update()
{
    if (! window_state.prepare_window()) return;

    if (! Logger::instance().empty()) {
        m_buffer.appendf("%s", Logger::instance().flush().c_str());
        m_scroll_to_bottom = 2;
    }

    ImGui::Begin("Log", &window_state.open);
        if (ImGui::Button("Clear")) {
            m_buffer.clear();
            m_line_offsets.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy")) {
            ImGui::LogToClipboard();
        }
        ImGui::SameLine();
        m_filter.Draw("Filter", -100.0f);
        ImGui::Separator();

        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
            if (m_filter.IsActive()) {
                const char* buf_begin = m_buffer.begin();
                const char* line = buf_begin;

                for (int line_no = 0; line != NULL; line_no++) {
                    const char* line_end = (line_no < m_line_offsets.Size) ? buf_begin + m_line_offsets[line_no] : NULL;

                    if (m_filter.PassFilter(line, line_end)) {
                        ImGui::TextUnformatted(line, line_end);
                    }

                    line = line_end && line_end[1] ? line_end + 1 : NULL;
                }
            } else {
                ImGui::TextUnformatted(m_buffer.begin());
            }

            if (m_scroll_to_bottom > 0) {
                m_scroll_to_bottom--;
                ImGui::SetScrollHere(1.0f);
            }
        ImGui::EndChild();
    ImGui::End();
}

