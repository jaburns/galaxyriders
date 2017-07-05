#version 150

in vec3 position;

out vec2 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 sprite_frame;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    v_tex_coords = vec2(
        mix(sprite_frame.x, sprite_frame.z, position.x),
        mix(sprite_frame.y, sprite_frame.w, 1 - position.y)
    );
}  