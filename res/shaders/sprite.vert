#version 150

in vec3 position;

out vec2 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 sprite_source;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    v_tex_coords = vec2(
        mix(sprite_source.x, sprite_source.z, position.x),
        mix(sprite_source.y, sprite_source.w, 1 - position.y)
    );
}  