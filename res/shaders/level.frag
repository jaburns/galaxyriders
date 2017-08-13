#version 150

in vec4 v_world_pos;
in vec3 v_surface_info;

out vec4 color;

uniform sampler2D ground_texture;
uniform sampler2D noise_texture;

void main()
{
    float one_minus_gnd = 1 - v_surface_info.z;
    float round_off = 1 - sqrt(1 - one_minus_gnd*one_minus_gnd);
    
    vec3 color_raw_ground = texture(ground_texture, v_world_pos.xy * 0.05 + vec2(round_off)).rgb;
    float color_noise = texture(noise_texture, v_world_pos.xy * 0.1 + round_off * v_surface_info.xy).r;
    vec3 color_dirt = (0.8 + 0.2*color_noise)*color_raw_ground;

    // TODO clamp roundoff to prevent the single pixel of black aliasing in and out.
    float lightness = 1 - 0.75*round_off; // *dot(normalize(vec2(1,-1)),v_surface_info.xy);

    color = vec4(lightness * color_dirt, 1);
}
