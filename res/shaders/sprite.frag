#version 150

in vec2 v_tex_coords;

out vec4 color;

uniform vec4 sprite_source;
uniform sampler2D sprite_texture;

void main()
{
    if (v_tex_coords.x < sprite_source.x || v_tex_coords.x > sprite_source.z
    ||  v_tex_coords.y < sprite_source.y || v_tex_coords.y > sprite_source.w) {
        color = vec4(0);
    } else {
        color = texture(sprite_texture, v_tex_coords);
    }
}