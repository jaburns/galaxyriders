use std::collections::HashMap;
use std::any::Any;
use std::any::TypeId;

#[derive(Default)]
pub struct Transform {
    pub position: [f32; 3],
}

#[derive(Default)]
pub struct World {
    component_lists: HashMap<TypeId, Box<Any>>,
}

impl World {
    pub fn new() -> World {
        Default::default()
    }

    pub fn add_component_list<T>(&mut self, starting_item: T)
    where T: 'static {
        let vec: Vec<T> = vec![starting_item];
        self.component_lists.insert(TypeId::of::<T>(), Box::new(vec));
    }

    pub fn run_system<T, F>(&mut self, func: F) 
    where T: 'static, F: Fn(&Vec<T>) {
        if let Some(any_val) = self.component_lists.get(&TypeId::of::<T>()) {
            let any_ref = any_val.as_ref();
            if let Some(vec) = any_ref.downcast_ref::<Vec<T>>() {
                func(&vec);
            }
        }
    }
}

pub fn ecs_test() {
    let mut world = World::new();

    world.add_component_list::<Transform>(Transform { position: [1.0,2.0,3.0] });

    world.run_system(|ts: &Vec<Transform>| {
        for t in ts {
            println!("Transform: {} {} {}", t.position[0], t.position[1], t.position[2]);
        }
    });
}