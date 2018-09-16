#pragma once

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

    static const EditorState NO_EDITOR;
};