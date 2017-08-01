#include "readinput.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <cmath>
#include <vector>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/vec4.hpp>

// Needed for g_projection_matrix and view matrix.
#include "render.hpp"

#define PI 3.14159f
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

    const auto x = 2.0f * xpos / fwidth - 1.0f;
    const auto y = 1.0f - 2.0f * ypos / fheight;

    glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::vec4 ray_eye = glm::inverse(Renderer::g_projection_matrix) * ray_clip;
    ray_eye.z = -1.0f;
    ray_eye.w =  0.0f;

    glm::vec4 ray_world = glm::inverse(Renderer::g_view_matrix) * ray_eye;
    ray_world.w = 0.0f;
    ray_world = glm::normalize(ray_world);

    state.mouse_ray = ray_world;
}

void Input::bind_handlers(GLFWwindow* window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
}

InputState Input::read_state()
{
    return state;
}
