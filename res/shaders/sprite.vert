#version 150

in vec3 position;

out vec3 v_position;
out vec2 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 sprite_source;
uniform vec4 sprite_frame;

void main()
{
    v_position = vec3(model * vec4(position, 1.0f));
    gl_Position = projection * view * model * vec4(position, 1.0f);

    v_tex_coords = vec2(
        mix(sprite_source.x, sprite_source.z, position.x),
        mix(sprite_source.y, sprite_source.w, position.y)
    );
}  