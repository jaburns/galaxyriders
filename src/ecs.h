#include <vector>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

using namespace std;

namespace ecs
{
    typedef unsigned int EntityId;

    struct Transform {
        vec3 position;
    };

    struct Renderer {
        float data;
    };

    struct PlayerInfo {
        float data;
    };

    struct ComponentSetBase { };

    template<typename T>
    struct ComponentSet : public ComponentSetBase {
        unordered_map<EntityId, T> components;
    };

    class World;
    class System;

    class EntityBuilder {
        EntityId entity_id;
        World *world;

    public:
        EntityBuilder(EntityId id, World *world);
        template<typename T>
            void add_component(T val);
    };

    class World {
        friend class EntityBuilder;

        EntityId next_entity_id;
        unordered_map<type_index,shared_ptr<ComponentSetBase>> component_sets;

        template<typename T>
            void add_component_for_entity(EntityId id, T val);

        template<typename S, typename T, typename U>
            void system_iter(S *sys, EntityId id, T &base_comp);

        template<typename S, typename T, typename U, typename V>
            void system_iter(S *sys, EntityId id, T &base_comp);

        template<typename S, typename T, typename U, typename V, typename W>
            void system_iter(S *sys, EntityId id, T &base_comp);

    public:
        World();
        EntityBuilder create_entity();

        template<typename S, typename T>
            void run_system(S *sys);

        template<typename S, typename T, typename U, typename ...Rest>
            void run_system(S *sys);
    };
}

using namespace ecs;

EntityBuilder::EntityBuilder(EntityId id, World *world)
{
    this->entity_id = id;
    this->world = world;
}

template<typename T>
void EntityBuilder::add_component(T val)
{
    world->add_component_for_entity(this->entity_id, val);
}

World::World()
{
    this->next_entity_id = 0;
}

template<typename T>
void World::add_component_for_entity(EntityId id, T val)
{
    auto has_T = component_sets.find(typeid(T));
    if (has_T == component_sets.end()) {
        component_sets[typeid(T)] = shared_ptr<ComponentSet<T>>(new ComponentSet<T>());
    }

    auto component_set = static_pointer_cast<ComponentSet<T>>(component_sets[typeid(T)]);
    component_set->components[id] = val;
}


EntityBuilder World::create_entity()
{
    return EntityBuilder(this->next_entity_id++, this);
}

template<typename S, typename T>
void World::run_system(S *sys)
{
    auto set_T = static_pointer_cast<ComponentSet<T>>(component_sets[typeid(T)]);
    for (auto &pair : set_T->components) {
        sys->run(pair.second);
    }
}

template<typename S, typename T, typename U, typename ...Rest>
void World::run_system(S *sys)
{
    auto set_T = static_pointer_cast<ComponentSet<T>>(component_sets[typeid(T)]);
    for (auto &pair : set_T->components) {
        system_iter<S,T,U,Rest...>(sys, pair.first, pair.second);
    }
}

template<typename S, typename T, typename U>
void World::system_iter(S *sys, EntityId id, T &base_comp)
{
    auto set_U = static_pointer_cast<ComponentSet<U>>(component_sets[typeid(U)])->components;
    auto has_U = set_U.find(id);
    if (has_U == set_U.end()) return;

    sys->run(base_comp, set_U[id]);
}

template<typename S, typename T, typename U, typename V>
void World::system_iter(S *sys, EntityId id, T &base_comp)
{
    auto set_U = static_pointer_cast<ComponentSet<U>>(component_sets[typeid(U)])->components;
    auto has_U = set_U.find(id);
    if (has_U == set_U.end()) return;

    auto set_V = static_pointer_cast<ComponentSet<V>>(component_sets[typeid(V)])->components;
    auto has_V = set_V.find(id);
    if (has_V == set_V.end()) return;

    sys->run(base_comp, set_U[id], set_V[id]);
}

template<typename S, typename T, typename U, typename V, typename W>
void World::system_iter(S *sys, EntityId id, T &base_comp)
{
    auto set_U = static_pointer_cast<ComponentSet<U>>(component_sets[typeid(U)])->components;
    auto has_U = set_U.find(id);
    if (has_U == set_U.end()) return;

    auto set_V = static_pointer_cast<ComponentSet<V>>(component_sets[typeid(V)])->components;
    auto has_V = set_V.find(id);
    if (has_V == set_V.end()) return;

    auto set_W = static_pointer_cast<ComponentSet<W>>(component_sets[typeid(W)])->components;
    auto has_W = set_W.find(id);
    if (has_W == set_W.end()) return;

    sys->run(base_comp, set_U[id], set_V[id], set_W[id]);
}


    struct MySystem
    {
        void run_with(World& w) {
            w.run_system<MySystem, Transform>(this);
        }

        void run(Transform& t) {
            cout << t.position[0] << " " << t.position[1] << endl;
        }
    };

    struct BigSystem
    {
        void run_with(World& w) {
            w.run_system<BigSystem, Transform, Renderer, PlayerInfo>(this);
        }

        void run(Transform& t, Renderer& h, PlayerInfo &b) {
            cout << endl;
            cout << t.position[0] << " <-> " << t.position[1] << endl;
            cout << h.data << endl;
            cout << b.data << endl;
            cout << endl;
        }
    };

    void ecs_test()
    {
        World w;

        Transform t = { { 1.0, 2.0, 3.0 } };
        Renderer h = { 91.0 };
        PlayerInfo b = { 22.0 };
        auto eb = w.create_entity();
        eb.add_component(t);
        eb.add_component(h);
        eb.add_component(b);

        auto eb2 = w.create_entity();
        eb2.add_component(t);

        BigSystem s;
        s.run_with(w);

        cout << "hello world" << endl;
    }
