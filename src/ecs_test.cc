#include <iostream>
#include "deps/linmath.h"
#include "ecs.h"

using namespace std;
using namespace ecs;


struct Transform {
    vec3 position;
};

struct MySystem : System<Transform>
{
    void run(EntityContext &c, Transform &t) {
        cout << t.position[0] << " " << t.position[1] << " " << t.position[2] << endl;
    }
};

void ecs_test()
{
    World w;

    auto eb = w.create_entity();
    eb.add_component((Transform){ { 0.0, 0.0, -2.0 } });

    MySystem s;
    w.run_system(s);
}
