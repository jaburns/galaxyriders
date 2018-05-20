#pragma once

struct EditorState
{
    bool wireframe = false;
    bool paused = false;

    static const EditorState NO_EDITOR;
};