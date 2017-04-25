use std::collections::HashMap;
use std::borrow::BorrowMut;
use std::any::Any;
use std::any::TypeId;

// -------------------------------------------------

pub struct Transform {
    pub position: [f32; 3],
}

// -------------------------------------------------

#[derive(Default,Copy,Clone)]
struct EntityID(u32);

struct ComponentSet<T> {
    pub vec_of_all: Vec<ComponentEntry<T>>,
}

struct ComponentEntry<T> {
    pub entity: EntityID,
    pub component: T,
}

#[derive(Default)]
pub struct World {
    cur_entity_id: EntityID,
    component_lists: HashMap<TypeId, Box<Any>>,  // Any -> ComponentSet<T>
}

pub struct EntityBuilder<'a> {
    entity_id: EntityID,
    world: &'a mut World,
}

impl<'a> EntityBuilder<'a> {
    pub fn add<T>(&mut self, component: T) -> &mut EntityBuilder<'a>
    where T: 'static {
        self.world.add_component(self.entity_id, component);
        self
    }
}

impl World {
    pub fn new() -> World {
        Default::default()
    }

    fn register_component_type<T>(&mut self)
    where T: 'static {
        let component_entry: ComponentSet<T> = ComponentSet {
            vec_of_all: Vec::new(),
        };
        self.component_lists.insert(TypeId::of::<T>(), Box::new(component_entry));
    }

    fn get_component_set<T>(&mut self) -> Option<&mut ComponentSet<T>>
    where T: 'static {
        if let Some(any_val) = self.component_lists.get_mut(&TypeId::of::<T>()) {
            let any_ref: &mut Any = any_val.borrow_mut();
            any_ref.downcast_mut::<ComponentSet<T>>()
        } else {
            None
        }
    }

    pub fn new_entity(&mut self) -> EntityBuilder {
        let EntityID(id_val) = self.cur_entity_id;
        self.cur_entity_id = EntityID(id_val + 1);

        EntityBuilder {
            entity_id: self.cur_entity_id,
            world: self
        }
    }

    fn add_component<T>(&mut self, entity_id: EntityID, comp: T)
    where T: 'static {
        if let Some(component_set) = self.get_component_set::<T>() {
            component_set.vec_of_all.push(ComponentEntry { entity: entity_id, component: comp });
            return;
        };

        self.register_component_type::<T>();
        if let Some(component_set) = self.get_component_set::<T>() {
            component_set.vec_of_all.push(ComponentEntry { entity: entity_id, component: comp });
        }
    }

    pub fn iter_component<T, F>(&mut self, func: &mut F) 
    where T: 'static, F: FnMut(&mut T) {
        if let Some(component_set) = self.get_component_set::<T>() {
            for single_component in component_set.vec_of_all.iter_mut() {
                func(&mut single_component.component);
            }
        }
    }
}

// -------------------------------------------------