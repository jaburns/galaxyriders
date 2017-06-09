#pragma once

#include "glfw.hpp"
#include "../shared/input.hpp"

namespace Input
{
    void bind_handlers(GLFWwindow* window);
    InputState read_state();
}
