#include "readinput.hpp"

#include <cmath>
#include <unordered_set>
#include <glm/vec4.hpp>

#define MAX_KEY_CODE 350

static constexpr float MOVE_SPEED = 0.05f;


// TODO roll into Core

static std::unordered_set<SDL_Keycode> keys_down;
static InputState state;

void Input::GLOBAL_key_callback(SDL_Keycode keycode, bool press)
{
    if (press) {
        keys_down.insert(keycode);
    } else {
        keys_down.erase(keycode);
    }

    state.shared.left  = keys_down.count(SDLK_LEFT);
    state.shared.right = keys_down.count(SDLK_RIGHT);
    state.shared.up    = keys_down.count(SDLK_UP);
    state.shared.down  = keys_down.count(SDLK_DOWN);
    state.debug_pause  = keys_down.count(SDLK_p);
    state.debug_step   = keys_down.count(SDLK_PERIOD);
}

static void mouse_button_callback(SDL_Window* window, int button, int action, int mods)
{
//  if (button == GLFW_MOUSE_BUTTON_LEFT) {
//      if (action == GLFW_PRESS) {
//          state.mouse_click = true;
//      } else if (action == GLFW_RELEASE) {
//          state.mouse_click = false;
//      }
//  }
}

static void cursor_pos_callback(SDL_Window* window, double xpos, double ypos)
{
//  int width, height;
//  glfwGetFramebufferSize(window, &width, &height);

//  const auto fwidth  = static_cast<float>(width);
//  const auto fheight = static_cast<float>(height);

//  state.mouse_pos = glm::vec2(
//      2.0f * xpos / fwidth - 1.0f,
//      1.0f - 2.0f * ypos / fheight
//  );
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

void Input::bind_handlers(SDL_Window* window)
{
//  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
//  glfwSetCursorPosCallback(window, cursor_pos_callback);
//  glfwSetMouseButtonCallback(window, mouse_button_callback);
//  glfwSetKeyCallback(window, key_callback);
}

InputState Input::read_state()
{
    return state;
}
