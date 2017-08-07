#version 150

in vec4 v_world_pos;
in vec2 v_surface_pos;

out vec4 color;

uniform sampler2D ground_texture;
uniform sampler2D noise_texture;

void main()
{
    vec3 color_raw_ground = texture(ground_texture, v_world_pos.xy * 0.1).rgb;
    float color_noise = texture(noise_texture, v_world_pos.xy * 0.1).r;
    vec3 color_dirt = (0.5 + 0.5*color_noise)*color_raw_ground;

    vec3 color_turf = mod(v_surface_pos.x, 1) > 0.5 ? vec3(1,0,0) : vec3(0,1,0);

    float antialias = fwidth(v_surface_pos.y);
    float smooth_depth = smoothstep(0, antialias, v_surface_pos.y);
    color = vec4(mix(color_turf, color_dirt, clamp(smooth_depth, 0.2, 0.8)), 1);
//  color = vec4(color_turf, 1);
}
