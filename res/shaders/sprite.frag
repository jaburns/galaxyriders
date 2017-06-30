#version 150

in vec3 v_position;
in vec2 v_tex_coords;

out vec4 color;

uniform sampler2D sprite_texture;

void main()
{
    color = vec4(v_tex_coords, 1.0, 1.0);
}