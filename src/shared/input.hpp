#pragma once

#include "fixed32.hpp"

struct InputState
{
    fixed32::vec3 look_dir = { fixed32::ZERO, fixed32::ZERO, fixed32::MINUS_ONE };
    fixed32::vec3 movement = { fixed32::ZERO, fixed32::ZERO, fixed32::ZERO };
    bool clicking = false;
};