#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "glfw.hpp"
#include "../shared/input.hpp"

struct InputState
{
    glm::vec2 mouse_pos;
    bool mouse_click;
    bool debug_pause;
    bool debug_step;

    SharedInputState shared;
};

namespace Input
{
    void bind_handlers(SDL_Window* window);
    InputState read_state();
    glm::vec3 get_mouse_ray(const glm::vec2& mouse_pos, const glm::mat4x4& projection, const glm::mat4x4& view);
}
