#ifndef __ecs_h
#define __ecs_h

#include <vector>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

using namespace std;

namespace ecs
{
    typedef unsigned int EntityId;

    struct ComponentSetBase { };

    template<typename T>
    struct ComponentSet : public ComponentSetBase {
        unordered_map<EntityId, T> components;
    };

    class World;

    class EntityContext
    {
        EntityId entity_id;
        World *world;

    public:
        EntityContext(EntityId id, World *world) {
            this->entity_id = id;
            this->world = world;
        }

        template<typename T>
        void add_component(T val) {
            world->add_component_for_entity(this->entity_id, val);
        }

        template<typename T>
        T* get_component() {
            auto set_T = static_pointer_cast<ComponentSet<T>>(world->component_sets[typeid(T)])->components;
            auto has_T = set_T.find(entity_id);
            if (has_T != set_T.end()) {
                return &set_T[entity_id];
            }
            return NULL;
        }
    };

    class World
    {
        friend class EntityContext;

        EntityId next_entity_id;
        unordered_map<type_index,shared_ptr<ComponentSetBase>> component_sets;

        template<typename T>
        void add_component_for_entity(EntityId id, T val) {
            auto has_T = component_sets.find(typeid(T));
            if (has_T == component_sets.end()) {
                component_sets[typeid(T)] = shared_ptr<ComponentSet<T>>(new ComponentSet<T>());
            }

            auto component_set = static_pointer_cast<ComponentSet<T>>(component_sets[typeid(T)]);
            component_set->components[id] = val;
        }

    public:
        World() {
            this->next_entity_id = 0;
        }

        EntityContext create_entity() {
            return EntityContext(this->next_entity_id++, this);
        }

        template<typename S> void run_system(S *sys) {
            typedef typename S::MainComponent MC;

            auto set_T = static_pointer_cast<ComponentSet<MC>>(component_sets[typeid(MC)]);

            for (auto &pair : set_T->components) {
                EntityContext ec(pair.first, this);
                sys->run(ec, pair.second);
            }
        }
    };

    template<typename T>
    struct System
    {
        typedef T MainComponent;
        void run(EntityContext &c, T &t);
    };
}

#endif
