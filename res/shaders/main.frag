#version 150

in vec3 v_normal;
in vec3 v_position;
in vec2 v_tex_coords;

out vec4 color;

uniform vec3 camera_pos;
uniform samplerCube skybox;
uniform sampler2D surf_texture;

const vec3 LIGHT_VEC = vec3(-0.7071, 0.7071, 0.0);

void main()
{
    vec3 norm_v_norm = normalize(v_normal);
    vec3 norm_view_vec = normalize(camera_pos - v_position);

    vec4 texture_color = texture(surf_texture, v_tex_coords);
    vec4 reflect_color = texture(skybox, reflect(-norm_view_vec, norm_v_norm));

    float cosine_light_normal = clamp(dot(LIGHT_VEC, norm_v_norm), 0.0, 1.0);
    float cosine_view_normal = clamp(dot(norm_view_vec, norm_v_norm), 0.0, 1.0);

    vec4 flat_color = texture_color * cosine_light_normal 
        + texture_color * cosine_view_normal; // specular term

    flat_color *= 0.9;

    color = mix(flat_color, reflect_color, 0.2);
}
