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
    bool debug_zoom_in;
    bool debug_zoom_out;

    SharedInputState shared;
};

namespace Core
{
    extern bool g_should_close_window;

    void init();
    void flip_frame_and_poll_events();
    void deinit();

    InputState read_input_state();

    const glm::mat4x4& get_perspective_matrix();
    glm::mat4x4 get_view_matrix(const glm::vec3& camera_pos);
    glm::vec2 get_mouse_world_pos(const glm::vec3& camera_pos, const glm::vec2& mouse_pos, const glm::mat4x4& projection, const glm::mat4x4& view);
};
