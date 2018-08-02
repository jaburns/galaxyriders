#pragma once

struct EditorState
{
    bool wireframe = false;
    bool paused = false;
    bool single_frame_step = false;

    static const EditorState NO_EDITOR;
};