#include "input.hpp"

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <math.h>
#define GLM_ENABLE_EXPERIMENTAL
#   include <glm/gtx/rotate_vector.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#define PI 3.14159f

const float MOVE_SPEED = 0.05f;

static bool key_w = false;
static bool key_s = false;
static bool key_a = false;
static bool key_d = false;
static bool key_space = false;
static bool key_lshift = false;

static float last_mouse_read_x = 0.0f;
static float last_mouse_read_y = 0.0f;
static float this_mouse_read_x = 0.0f;
static float this_mouse_read_y = 0.0f;

static float facing = 0.0f;
static float tilt = 0.0f;

static InputState state;

static void update_state()
{
    state.look_dir = {
        cos(facing - glm::half_pi<float>()),
        0.0f,
        sin(facing - glm::half_pi<float>())
    };
    glm::vec3 side_dir = { -state.look_dir.z, 0.0f, state.look_dir.x };

    state.movement = { 0.0f, 0.0f, 0.0f };

    if (key_w) state.movement += MOVE_SPEED * state.look_dir;
    if (key_s) state.movement -= MOVE_SPEED * state.look_dir;
    if (key_a) state.movement -= MOVE_SPEED * side_dir;
    if (key_d) state.movement += MOVE_SPEED * side_dir;

    state.look_dir.y = tilt;
    if (key_space)  state.movement.y += MOVE_SPEED;
    if (key_lshift) state.movement.y -= MOVE_SPEED;
}

static void update_key(bool& out_var, int match_key, int key, int action)
{
    if (key == match_key) {
        if (action == GLFW_PRESS) out_var = true;
        else if (action == GLFW_RELEASE) out_var = false;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    update_key(key_w, GLFW_KEY_W, key, action);
    update_key(key_s, GLFW_KEY_S, key, action);
    update_key(key_a, GLFW_KEY_A, key, action);
    update_key(key_d, GLFW_KEY_D, key, action);
    update_key(key_space, GLFW_KEY_SPACE, key, action);
    update_key(key_lshift, GLFW_KEY_LEFT_SHIFT, key, action);

    update_state();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            state.clicking = true;
        } else if (action == GLFW_RELEASE) {
            state.clicking = false;
        }
    }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    this_mouse_read_x = (float)xpos;
    this_mouse_read_y = (float)ypos;

    auto dx = this_mouse_read_x - last_mouse_read_x;
    auto dy = this_mouse_read_y - last_mouse_read_y;

    facing += dx / 1000.0f;
    tilt -= dy / 1000.0f;
    if (tilt < -2.0f) tilt = -2.0f;
    if (tilt >  2.0f) tilt =  2.0f;

    last_mouse_read_x = this_mouse_read_x;
    last_mouse_read_y = this_mouse_read_y;

    update_state();
}

static void cursor_pos_callback_first(GLFWwindow* window, double xpos, double ypos)
{
    last_mouse_read_x = (float)xpos;
    last_mouse_read_y = (float)ypos;
    glfwSetCursorPosCallback(window, cursor_pos_callback);
}

void Input::bind_handlers(GLFWwindow* window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_pos_callback_first);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    update_state();
}

InputState Input::read_state()
{
    return state;
}
