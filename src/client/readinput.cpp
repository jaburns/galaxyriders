#include "readinput.hpp"

#include <cmath>
#include <vector>
#include <glm/vec4.hpp>

#define MAX_KEY_CODE 350

const float MOVE_SPEED = 0.05f;

static bool key_down[MAX_KEY_CODE] = {};
static InputState state;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        key_down[key] = true;
    } else if (action == GLFW_RELEASE) {
        key_down[key] = false;
    }

    if (key_down[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    state.shared.left  = key_down[GLFW_KEY_LEFT];
    state.shared.right = key_down[GLFW_KEY_RIGHT];
    state.shared.up    = key_down[GLFW_KEY_UP];
    state.shared.down  = key_down[GLFW_KEY_DOWN];
    state.debug_pause  = key_down[GLFW_KEY_P];
    state.debug_step   = key_down[GLFW_KEY_PERIOD];
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            state.mouse_click = true;
        } else if (action == GLFW_RELEASE) {
            state.mouse_click = false;
        }
    }
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    const auto fwidth  = static_cast<float>(width);
    const auto fheight = static_cast<float>(height);

    state.mouse_pos = glm::vec2(
        2.0f * xpos / fwidth - 1.0f,
        1.0f - 2.0f * ypos / fheight
    );
}

glm::vec3 Input::get_mouse_ray(const glm::vec2& mouse_pos, const glm::mat4x4& projection, const glm::mat4x4& view)
{
    glm::vec4 ray_clip = glm::vec4(mouse_pos, -1.0f, 1.0f);

    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye.z = -1.0f;
    ray_eye.w =  0.0f;

    glm::vec4 ray_world = glm::inverse(view) * ray_eye;
    ray_world.w = 0.0f;
    ray_world = glm::normalize(ray_world);

    return ray_world;
}

void Input::bind_handlers(GLFWwindow* window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
}

InputState Input::read_state()
{
    return state;
}
