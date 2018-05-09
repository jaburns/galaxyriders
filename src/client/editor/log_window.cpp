#include "log_window.hpp"

#include "../../shared/logger.hpp"

static bool prepare_window(WindowState& state)
{
    if (! state.open) return false;

    if (state.should_reset) {
        ImGui::SetNextWindowSize(ImVec2(400.0f, 300.0f));
        ImGui::SetNextWindowPosCenter();
        state.should_reset = false;
    }

    return true;
}

void LogWindow::update()
{
    if (!prepare_window(window_state)) return;

    if (! Logger::instance().empty()) {
        buffer.appendf("%s", Logger::instance().flush().c_str());
        scrollToBottom = 2;
    }

    ImGui::Begin("Log", &window_state.open);
        if (ImGui::Button("Clear")) {
            buffer.clear();
            lineOffsets.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy")) {
            ImGui::LogToClipboard();
        }
        ImGui::SameLine();
        filter.Draw("Filter", -100.0f);
        ImGui::Separator();

        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
            if (filter.IsActive()) {
                const char* buf_begin = buffer.begin();
                const char* line = buf_begin;

                for (int line_no = 0; line != NULL; line_no++) {
                    const char* line_end = (line_no < lineOffsets.Size) ? buf_begin + lineOffsets[line_no] : NULL;

                    if (filter.PassFilter(line, line_end)) {
                        ImGui::TextUnformatted(line, line_end);
                    }

                    line = line_end && line_end[1] ? line_end + 1 : NULL;
                }
            } else {
                ImGui::TextUnformatted(buffer.begin());
            }

            if (scrollToBottom > 0) {
                scrollToBottom--;
                ImGui::SetScrollHere(1.0f);
            }
        ImGui::EndChild();
    ImGui::End();
}

