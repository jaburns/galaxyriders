#version 150

in vec3 v_normal;
in vec3 v_position;

out vec4 color;

uniform vec3 camera_pos;
uniform samplerCube skybox;
uniform sampler2D surf_texture;

void main()
{
    vec3 I = normalize(v_position - camera_pos);
    vec3 R = reflect(I, normalize(v_normal));
    color = texture(skybox, R);
}
