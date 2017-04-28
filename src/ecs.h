#ifndef __ecs_h
#define __ecs_h

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace ecs
{
    typedef unsigned int EntityId;

    struct ComponentSetBase { };

    template<typename T>
    struct ComponentSet : public ComponentSetBase {
        std::unordered_map<EntityId, T> components;
    };

    class World;

    class EntityContext
    {
        EntityId entity_id;
        World *world;

    public:
        EntityContext(EntityId id, World *world)
            : entity_id(id), world(world)
        { }

        template<typename T> void add_component(T val);
        template<typename T> T* get_component();
    };

    class World
    {
        friend class EntityContext;

        EntityId next_entity_id;
        std::unordered_map<std::type_index, std::shared_ptr<ComponentSetBase>> component_sets;

        template<typename T> void add_component_for_entity(EntityId id, T val);

    public:
        World()
            : next_entity_id(0)
        { }

        EntityContext create_entity();

        template<typename S> void run_system(S& sys);
    };

    template<typename T>
    struct System
    {
        typedef T MainComponent;
        void run(EntityContext& c, T& t);
    };

// ------------------------------------------------------------------------

    template<typename T>
    void EntityContext::add_component(T val)
    {
        world->add_component_for_entity(entity_id, val);
    }

    template<typename T>
    T* EntityContext::get_component()
    {
        auto set_T = std::static_pointer_cast<ComponentSet<T>>(world->component_sets[typeid(T)])->components;
        auto has_T = set_T.find(entity_id);
        if (has_T != set_T.end()) {
            return &set_T[entity_id];
        }
        return NULL;
    }


    template<typename T>
    void World::add_component_for_entity(EntityId id, T val)
    {
        auto has_T = component_sets.find(typeid(T));
        if (has_T == component_sets.end()) {
            component_sets[typeid(T)] = std::shared_ptr<ComponentSet<T>>(new ComponentSet<T>());
        }

        auto component_set = std::static_pointer_cast<ComponentSet<T>>(component_sets[typeid(T)]);
        component_set->components[id] = val;
    }

    EntityContext World::create_entity() 
    {
        return EntityContext(next_entity_id++, this);
    }

    template<typename S>
    void World::run_system(S& sys)
    {
        typedef typename S::MainComponent MC;

        auto set_T = std::static_pointer_cast<ComponentSet<MC>>(component_sets[typeid(MC)]);

        for (auto& pair : set_T->components) {
            EntityContext ec(pair.first, this);
            sys.run(ec, pair.second);
        }
    }
}

#endif
