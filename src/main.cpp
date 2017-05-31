#include "glfw.hpp"
#include "input.hpp"
#include "render.hpp"

#include <iostream>

extern int server_main(int argc, char** argv);
extern int client_main(int argc, char** argv);

using namespace std;

void run()
{
    Renderer renderer;
    World world;

    Input::bind_handlers(renderer.raw_glfw_window());

    while (!renderer.should_close_window()) {
        auto input_state = Input::read_state();
        world = world.step(input_state);
        renderer.render(world);
    }
}

int main(int argc, char** argv)
{
    client_main(argc, argv);

//  run();

    cout << "Enter a char to quit";
    char x;
    cin >> x;

    return 0;
}
