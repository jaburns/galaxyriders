#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../shared/input.hpp"

struct InputState
{
    glm::vec2 mouse_pos;
    bool mouse_click;
    bool debug_pause;
    bool debug_step;

    SharedInputState shared;
};

namespace Core 
{
    extern bool g_should_close_window;
    extern int g_window_width, g_window_height;

    void init();

    void flip_frame_and_poll_events();

    InputState read_input_state();
    glm::vec3 get_mouse_ray(const glm::vec2& mouse_pos, const glm::mat4x4& projection, const glm::mat4x4& view);

    void deinit();
};