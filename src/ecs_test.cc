#include <iostream>
#include "deps/linmath.h"
#include "ecs.h"

using namespace std;
using namespace ecs;


struct Transform {
    vec3 position;
};

struct Renderer {
    float data;
};

struct PlayerInfo {
    float data;
};

struct MySystem : System<Transform>
{
    void run(EntityContext &c, Transform &t) {
        cout << t.position[0] << " " << t.position[1] << " " << t.position[2] << endl;

        auto renderer = c.get_component<Renderer>();
        if (! renderer) return;

        cout << "RENDERER: " << renderer->data << endl;
    }
};

void ecs_test()
{
    World w;

    Transform t0 = { { 1.0, 2.0, 3.0 } };
    Transform t1 = { { 10.0, 21.0, 31.0 } };
    Renderer h = { 91.0 };
    PlayerInfo b = { 22.0 };
    auto eb = w.create_entity();
    eb.add_component(t0);
    eb.add_component(h);
    eb.add_component(b);

    auto eb2 = w.create_entity();
    eb2.add_component(t1);

    MySystem s;
    w.run_system(&s);
}
