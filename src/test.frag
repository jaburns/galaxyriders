#version 150

in vec3 v_normal;
in vec2 v_tex_coords;

out vec4 color;

uniform sampler2D tex;
uniform vec3 light;

void main() {
    float brightness = dot(normalize(v_normal), normalize(light));
    color = (0.5 + 0.5 * brightness) * texture(tex, v_tex_coords);
}