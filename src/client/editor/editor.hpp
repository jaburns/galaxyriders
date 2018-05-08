#pragma once

#include "editor_state.hpp"

class Editor
{
    EditorState m_state;

public:
    EditorState update();
};
