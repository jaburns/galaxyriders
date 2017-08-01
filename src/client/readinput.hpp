#pragma once

#include <glm/vec3.hpp>
#include "glfw.hpp"
#include "../shared/input.hpp"

struct InputState
{
    glm::vec3 mouse_ray = { 0.0f, 0.0f, -1.0f };
    bool mouse_click;
    bool debug_pause;
    bool debug_step;

    SharedInputState shared;
};

namespace Input
{
    void bind_handlers(GLFWwindow* window);
    InputState read_state();
}
