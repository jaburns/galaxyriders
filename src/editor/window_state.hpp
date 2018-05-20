#pragma once

struct WindowState
{
    bool open = false;
    bool should_reset = false;

    bool prepare_window();
};