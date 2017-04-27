#[macro_use]
extern crate glium;
extern crate image;

mod ecs;
mod teapot;

use std::io::Cursor;
use glium::{DisplayBuild,Surface,Program};

#[derive(Copy, Clone)]
struct Vertex {
    position: [f32; 2],
    tex_coords: [f32; 2],
}
implement_vertex!(Vertex, position, tex_coords);

fn get_perspective_matrix(screen_dimensions: (u32,u32)) -> [[f32; 4]; 4] {
    let (width, height) = screen_dimensions; //target.get_dimensions();
    let aspect_ratio = height as f32 / width as f32;

    let fov: f32 = 3.141592 / 3.0;
    let zfar = 1024.0;
    let znear = 0.1;
    let f = 1.0 / (fov / 2.0).tan();

    [
        [f * aspect_ratio, 0.0,              0.0              , 0.0],
        [       0.0      ,  f ,              0.0              , 0.0],
        [       0.0      , 0.0,      (zfar+znear)/(zfar-znear), 1.0],
        [       0.0      , 0.0, -(2.0*zfar*znear)/(zfar-znear), 0.0],
    ]
}

fn main() {
    let mut t: f32 = -0.5;

//  ecs::ecs_test();
//  return;

    let display = glium::glutin::WindowBuilder::new()
        .with_title("Game")
        .with_depth_buffer(24)
        .build_glium().unwrap();

    let positions = glium::VertexBuffer::new(&display, &teapot::VERTICES).unwrap();
    let normals = glium::VertexBuffer::new(&display, &teapot::NORMALS).unwrap();
    let indices = glium::IndexBuffer::new(&display, glium::index::PrimitiveType::TrianglesList, &teapot::INDICES).unwrap();

    let program = Program::from_source(&display, include_str!("test.vert"), include_str!("test.frag"), None).unwrap();

    let image = image::load(Cursor::new(&include_bytes!("../res/texture.png")[..]), image::PNG).unwrap().to_rgba();
    let image_dimensions = image.dimensions();
    let image = glium::texture::RawImage2d::from_raw_rgba_reversed(image.into_raw(), image_dimensions);
    let texture = glium::texture::Texture2d::new(&display, image).unwrap();

    let params = glium::DrawParameters {
        depth: glium::Depth {
            test: glium::draw_parameters::DepthTest::IfLess,
            write: true,
            .. Default::default()
        },
        backface_culling: glium::draw_parameters::BackfaceCullingMode::CullClockwise,
        .. Default::default()
    };

    let mut world = ecs::World::new();
    world.new_entity()
        .add(ecs::Transform { position: [0.0,0.0,2.0] });

    loop {
        t += 0.0008;
        if t > 3.14159 {
            t = -3.14159;
        }

        let mut target = display.draw();
        target.clear_color_and_depth((0.0, 0.0, 1.0, 1.0), 1.0);
        let perspective = get_perspective_matrix(target.get_dimensions());

//      world.iter_component(&mut |tr: &mut ecs::Transform| {
//          tr.position[1] = t;
//      });

        world.iter_component(&mut |tr: &mut ecs::Transform| {
            let uniforms = uniform! {
                perspective: perspective,
                matrix: [
                    [0.01 * t.cos(), 0.0, 0.01 * t.sin(), 0.0],
                    [0.0, 0.01, 0.0, 0.0],
                    [0.01 *-t.sin(), 0.0, 0.01 * t.cos(), 0.0],
                    [tr.position[0], tr.position[1], tr.position[2], 1.0f32],
                ],
                light: [-1.0, 0.4, 0.9f32],
                tex: &texture,
            };
            target.draw((&positions, &normals), &indices, &program, &uniforms, &params).unwrap();
        });

        target.finish().unwrap();

        for ev in display.poll_events() {
            match ev {
                glium::glutin::Event::Closed => return,   // the window has been closed by the user
                _ => ()
            }
        }
    }
}
