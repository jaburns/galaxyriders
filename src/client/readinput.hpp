#pragma once

#include <glm/vec3.hpp>
#include "glfw.hpp"
#include "../shared/input.hpp"

struct InputState
{
    glm::vec3 look_dir = { 0.0f, 0.0f, -1.0f };
    glm::vec3 movement = { 0.0f, 0.0f, 0.0f };
    bool clicking = false;

    SharedInputState shared;
};

namespace Input
{
    void bind_handlers(GLFWwindow* window);
    InputState read_state();
}
