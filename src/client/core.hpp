#pragma once

#include <unordered_set>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "gl.hpp"
#include "../shared/lang_utils.hpp"
#include "../shared/player_input.hpp"

struct InputState
{
    glm::vec2 mouse_pos = { 0.0f, 0.0f };
    glm::ivec2 mouse_scroll = { 0, 0 };
    bool mouse_click = false;
    PlayerInput player;
};

class CoreView
{
    glm::vec2 m_mouse_pos;
    glm::mat4x4 m_perspective;

public:
    CoreView(glm::vec2 mouse_pos, int window_width, int window_height);

    glm::mat4x4 get_perspective_matrix() const;
    glm::mat4x4 get_view_matrix(const glm::vec3& camera_pos) const;
    glm::vec2 get_mouse_world_pos(const glm::vec3& camera_pos) const;
};

class Core : public NoCopy
{
    SDL_Window *m_window;
    SDL_GLContext m_context;
    int m_window_width;
    int m_window_height;

    std::unordered_set<SDL_Keycode> m_keys_down;
    InputState m_input_state;

    void handle_key_event(SDL_Keycode keycode, bool press);
    void handle_mouse_motion(SDL_MouseMotionEvent event);

public:
    Core();
    ~Core();

    bool flip_frame_and_poll_events();

    InputState read_input_state() const;
    CoreView get_core_view() const;
};