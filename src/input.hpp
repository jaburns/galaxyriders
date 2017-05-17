#pragma once

#include <glm/vec3.hpp>
#include "glfw.hpp"

struct InputState
{
    glm::vec3 look_dir;
    glm::vec3 movement;
};

namespace Input
{
    void bind_handlers(GLFWwindow* window);
    InputState read_state();
}
