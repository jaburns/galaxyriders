#version 150

in float v_vdepth;

out vec4 color;

uniform vec3 color_dirt;
uniform vec3 color_turf;

void main()
{
    float antialias = 0.5*fwidth(v_vdepth);
    float smooth_depth = smoothstep(0.5 - antialias, 0.5 + antialias, v_vdepth);
    color = vec4(mix(color_turf, color_dirt, smooth_depth), 1.0);
}
