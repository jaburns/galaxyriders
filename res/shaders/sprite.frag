#version 150

in vec2 v_tex_coords;

out vec4 color;

uniform sampler2D sprite_texture;
uniform vec3 main_color;

void main()
{
    float dist = texture(sprite_texture, v_tex_coords).r;
    float antialias = 0.5*fwidth(dist);
    float alpha = smoothstep(0.5 - antialias, 0.5 + antialias, dist);
    color = vec4(main_color, alpha);
}