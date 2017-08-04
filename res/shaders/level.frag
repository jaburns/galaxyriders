#version 150

in vec4 v_world_pos;
in vec2 v_surface_pos;

out vec4 color;

uniform sampler2D ground_texture;
uniform sampler2D noise_texture;

void main()
{
    vec4 color_ground = texture(ground_texture, v_world_pos.xy * 0.1);
    float color_noise  = texture( noise_texture, v_world_pos.xy * 0.1).r;
    vec4 color_mixed = (0.5 + 0.5*color_noise)*color_ground;

    vec4 color_with_surf = mix(vec4(0,0,0.9,1), color_mixed, clamp(v_surface_pos.y, 0.2, 1));

    float clamper = clamp((1 - v_surface_pos.y)*10, 0, 1);
    float changer = (0.5 + 0.5*sin(v_surface_pos.x)) * clamper;

    color = mix(color_with_surf, vec4(1, 0, 0, 1), changer);
}
