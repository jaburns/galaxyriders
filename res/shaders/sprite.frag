#version 150

in vec3 v_position;
in vec2 v_tex_coords;

out vec4 color;

uniform sampler2D sprite_texture;

void main()
{
    color = texture(sprite_texture, v_tex_coords);
}