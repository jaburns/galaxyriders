#version 150

in vec2 v_tex_coords;

out vec4 color;

uniform sampler2D sprite_texture;

void main()
{
    vec4 tex_color = texture(sprite_texture, v_tex_coords);

    if (tex_color.r > 0.5) color = vec4(1); 
    else color = vec4(0);
}