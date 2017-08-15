#version 150

in vec4 v_world_pos;
in vec3 v_surface_info;

out vec4 color;

uniform sampler2D noise_texture;

const vec3 A = vec3(223.0 / 255.0, 61.0 / 255.0, 161.0 / 255.0);
const vec3 B = vec3(192.0 / 255.0, 52.0 / 255.0, 133.0 / 255.0);
const vec3 C = vec3(166.0 / 255.0, 55.0 / 255.0, 115.0 / 255.0);
const vec3 STRIPES[] = vec3[](A,B,C,B,A,C,B,C);

const vec2 LIGHT = normalize(vec2(-1.0, 2.0));
const float SWIRLY = 0.05;

vec3 stripes(vec2 xy)
{
    vec2 uv = mod(xy, vec2(1.0));
    float t = 8.0 * (uv.x + 2.0 - uv.y);
    float i, f = modf(t, i);

    if (f > 0.25 && f < 0.75) {
        float w = fwidth(f) / 2.0;
        return mix(STRIPES[int(i) % 8], STRIPES[int(i + 1.0) % 8], smoothstep(0.5 - w, 0.5 + w, f));
    }

    return STRIPES[int(t + 0.5) % 8];
}

void main()
{
    float one_minus_gnd = 1 - clamp(v_surface_info.z, fwidth(v_surface_info.z), 1.0);
    float round_off = 1 - sqrt(1 - one_minus_gnd*one_minus_gnd);

    float color_noise = texture(noise_texture, v_world_pos.xy * 0.1 + round_off * v_surface_info.xy).r;

    vec3 color_raw_ground = stripes(v_world_pos.xy * 0.05 + 0.2*vec2(-round_off, round_off) + SWIRLY*vec2(color_noise,-color_noise));
    vec3 color_dirt = (0.8 + 0.2*color_noise)*color_raw_ground;

    float light_add = round_off * dot(LIGHT, v_surface_info.xy);
    float lightness = 0.5 + 0.5*light_add;

    color = vec4(lightness * color_dirt, 1);
}
