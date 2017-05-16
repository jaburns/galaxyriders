#include "glfw.h"
#include "input.h"
#include "render.h"

using namespace std;

int main(void)
{
    Renderer renderer;
    World world;

    Input::bind_handlers(renderer.raw_glfw_window());

    while (!renderer.should_close_window()) {
        world = world.step(Input::read_state());
        renderer.render(world);
    }

    return 0;
}
