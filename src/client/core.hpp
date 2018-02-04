#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "../shared/input.hpp"

// Get Debug:: everywhere
#include "../shared/debug.hpp"

struct InputState
{
    glm::vec2 mouse_pos = { 0.0f, 0.0f };
    bool mouse_click = false;

    bool editmode_toggle = false;
    bool editmode_step = false;
    bool editmode_zoom_in = false;
    bool editmode_zoom_out = false;

    bool debug_toggle_wireframe = false;

    SharedInputState shared;
};

namespace Core
{
    void init();
    bool flip_frame_and_poll_events();
    void deinit();

    InputState read_input_state();

    const glm::mat4x4& get_perspective_matrix();
    glm::mat4x4 get_view_matrix(const glm::vec3& camera_pos);
    glm::vec2 get_mouse_world_pos(const glm::vec3& camera_pos, const glm::vec2& mouse_pos);
};
