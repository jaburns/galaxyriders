#include "deps/glfw.h"
#include "input.h"
#include "render.h"

using namespace std;

RawInput raw_input;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) raw_input.key_w = true;
        else if (action == GLFW_RELEASE) raw_input.key_w = false;
    }
    else if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) raw_input.key_s = true;
        else if (action == GLFW_RELEASE) raw_input.key_s = false;
    }
    else if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) raw_input.key_a = true;
        else if (action == GLFW_RELEASE) raw_input.key_a = false;
    }
    else if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) raw_input.key_d = true;
        else if (action == GLFW_RELEASE) raw_input.key_d = false;
    }
}

int main(void)
{
    Renderer renderer = Renderer(key_callback);
    World world;

    raw_input.key_w = false;
    raw_input.key_s = false;
    raw_input.key_a = false;
    raw_input.key_d = false;

    while (!renderer.should_close_window()) {
        world = world.step(Input(raw_input));
        renderer.render(world);
    }

    return 0;
}