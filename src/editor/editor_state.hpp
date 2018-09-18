#pragma once

#include <glm/vec2.hpp>

struct EditorState
{
    bool wireframe = false;
    bool paused = false;
    bool single_frame_step = false;

    struct HandleIndex
    {
        int poly = -1, handle = -1;
    };

    enum SelectedHandleState
    {
        Not, Hovered, Selected
    };

    HandleIndex selected_level_handle = { };
    SelectedHandleState selected_level_handle_state = Not;
    bool dragging_camera = false;
    glm::vec2 drag_camera_origin = { 0, 0 };
    glm::vec2 drag_mouse_origin = { 0, 0 };

    EditorState() { };

    static const EditorState NO_EDITOR;
};
