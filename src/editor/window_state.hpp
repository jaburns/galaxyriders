#pragma once
#include "../shared/global.hpp"

struct WindowState
{
    bool open = false;
    bool should_reset = false;

    bool prepare_window();
};