#version 150

in vec4 v_world_pos;
in vec3 v_surface_info;

out vec4 color;

uniform sampler2D ground_texture;
uniform sampler2D noise_texture;

const vec3 A = vec3(223. / 255., 61. / 255., 161. / 255.);
const vec3 B = vec3(192. / 255., 52. / 255., 133. / 255.);
const vec3 C = vec3(166. / 255., 55. / 255., 115. / 255.);

vec3 stripes(vec2 xy)
{
    vec2 uv = vec2(mod(xy.x, 1.0), mod(xy.y, 1.0));
    for (float f = 0.0f; f < 1.5f; f += 1.0f) {
        if (uv.x < f + uv.y - 0.875) return A;
        if (uv.x < f + uv.y - 0.750) return B;
        if (uv.x < f + uv.y - 0.625) return C;
        if (uv.x < f + uv.y - 0.500) return B;
        if (uv.x < f + uv.y - 0.375) return A;
        if (uv.x < f + uv.y - 0.250) return C;
        if (uv.x < f + uv.y - 0.125) return B;
        if (uv.x < f + uv.y        ) return C;
    }
}

vec3 get_ground_A(vec2 uv)
{
    return texture(ground_texture, uv).rgb;
}

void main()
{
    float one_minus_gnd = 1 - v_surface_info.z;
    float round_off = 1 - sqrt(1 - one_minus_gnd*one_minus_gnd);

    vec3 color_raw_ground = stripes(v_world_pos.xy * 0.05 + 0.2*vec2(-round_off, round_off));
    float color_noise = texture(noise_texture, v_world_pos.xy * 0.1 + round_off * v_surface_info.xy).r;
    vec3 color_dirt = (0.8 + 0.2*color_noise)*color_raw_ground;

    // TODO clamp roundoff to prevent the single pixel of black aliasing in and out.
    float lightness = 1 - 0.65*round_off; // *dot(normalize(vec2(1,-1)),v_surface_info.xy);

    color = vec4(lightness * color_dirt, 1);
}
