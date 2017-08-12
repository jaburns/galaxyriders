#version 150

in vec4 v_world_pos;
in vec3 v_surface_info;

out vec4 color;

uniform sampler2D ground_texture;
uniform sampler2D noise_texture;

void main()
{
    float groundy = 1 - v_surface_info.z;
    vec3 color_raw_ground = texture(ground_texture, v_world_pos.xy * 0.1 + 0.1 * v_surface_info.xy * (groundy + groundy*groundy)*0.5).rgb;
    float color_noise = texture(noise_texture, v_world_pos.xy * 0.1).r;
    vec3 color_dirt = (0.5 + 0.5*color_noise)*color_raw_ground;
    vec3 color_turf = vec3(0,1,0);

    color = vec4(mix(color_turf, color_dirt, v_surface_info.z/2 + 0.5), 1);

//  float antialias = fwidth(v_surface_pos.y);
}