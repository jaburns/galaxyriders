#version 150

in float v_vdepth;
in float v_vspan;
in vec4 v_world_pos;

out vec4 color;

uniform sampler2D ground_texture;
uniform sampler2D noise_texture;

void main()
{
//  float antialias = 0.5*fwidth(v_vdepth);
//  float smooth_depth = smoothstep(0.5 - antialias, 0.5 + antialias, v_vdepth);

// TODO draw something along v_vspan

    float smooth_depth = v_vdepth;

    vec4 color_ground = texture(ground_texture, v_world_pos.xy * 0.1);
    float color_noise  = texture( noise_texture, v_world_pos.xy * 0.1).r;
    vec4 color_mixed = (0.5 + 0.5*color_noise)*color_ground;

    color = mix(vec4(0.0,0.0,0.9,1.0), color_mixed, clamp(smooth_depth, 0.2, 1.0));
}
