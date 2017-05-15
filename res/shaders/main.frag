#version 150

const vec3 light = vec3(-1.0, 0.4, 0.9);

in vec2 v_tex_coords;
in vec3 v_normal;

out vec4 color;

uniform sampler2D tex;

void main()
{
    float brightness = dot(normalize(v_normal), normalize(light));
    color = (0.5 + 0.5 * brightness) * texture(tex, v_tex_coords);
}
