#[macro_use]
extern crate glium;
extern crate image;

mod teapot;

use std::io::Cursor;
use glium::{DisplayBuild,Surface};

#[derive(Copy, Clone)]
struct Vertex {
    position: [f32; 2],
    tex_coords: [f32; 2],
}

implement_vertex!(Vertex, position, tex_coords);

fn main() {
    let mut t: f32 = -0.5;

    let vertex_shader_src = r#"
        #version 140

        in vec3 position;
        in vec3 normal;

        out vec3 v_normal;
        out vec2 v_tex_coords;

        uniform mat4 perspective;
        uniform mat4 matrix;

        void main() {
            v_normal = transpose(inverse(mat3(matrix))) * normal;
            v_tex_coords = vec2(position.x * 0.01, position.y * 0.01);
            gl_Position = perspective * matrix * vec4(position, 1.0);
        }
    "#;

    let fragment_shader_src = r#"
        #version 140

        in vec3 v_normal;
        in vec2 v_tex_coords;

        out vec4 color;

        uniform sampler2D tex;
        uniform vec3 light;

        void main() {
            float brightness = dot(normalize(v_normal), normalize(light));
            color = (0.5 + 0.5 * brightness) * texture(tex, v_tex_coords);
        }
    "#;


    let display = glium::glutin::WindowBuilder::new()
        .with_depth_buffer(24)
        .build_glium().unwrap();

    let positions = glium::VertexBuffer::new(&display, &teapot::VERTICES).unwrap();
    let normals = glium::VertexBuffer::new(&display, &teapot::NORMALS).unwrap();
    let indices = glium::IndexBuffer::new(&display, glium::index::PrimitiveType::TrianglesList, &teapot::INDICES).unwrap();

    let program = glium::Program::from_source(&display, vertex_shader_src, fragment_shader_src, None).unwrap();

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

    loop {
        t += 0.0002;
        if t > 3.14159 {
            t = -3.14159;
        }

        let mut target = display.draw();
        target.clear_color_and_depth((0.0, 0.0, 1.0, 1.0), 1.0);

        let perspective = {
            let (width, height) = target.get_dimensions();
            let aspect_ratio = height as f32 / width as f32;

            let fov: f32 = 3.141592 / 3.0;
            let zfar = 1024.0;
            let znear = 0.1;

            let f = 1.0 / (fov / 2.0).tan();

            [
                [f *   aspect_ratio   ,    0.0,              0.0              ,   0.0],
                [         0.0         ,     f ,              0.0              ,   0.0],
                [         0.0         ,    0.0,  (zfar+znear)/(zfar-znear)    ,   1.0],
                [         0.0         ,    0.0, -(2.0*zfar*znear)/(zfar-znear),   0.0],
            ]
        };

        let uniforms = uniform! {
            perspective: perspective,
            matrix: [
                [0.01 * t.cos(), 0.0, 0.01 * t.sin(), 0.0],
                [0.0, 0.01, 0.0, 0.0],
                [0.01 *-t.sin(), 0.0, 0.01 * t.cos(), 0.0],
                [0.0, 0.0, 2.0, 1.0f32],
            ],
            light: [-1.0, 0.4, 0.9f32],
            tex: &texture,
        };
        target.draw((&positions, &normals), &indices, &program, &uniforms, &params).unwrap();
        target.finish().unwrap();

        for ev in display.poll_events() {
            match ev {
                glium::glutin::Event::Closed => return,   // the window has been closed by the user
                _ => ()
            }
        }
    }
}