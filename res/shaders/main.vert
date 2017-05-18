#version 150

in vec3 position;
in vec3 normal;

out vec3 v_normal;
out vec3 v_position;
out vec2 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    v_normal = mat3(transpose(inverse(model))) * normal;
    v_position = vec3(model * vec4(position, 1.0f));
    v_tex_coords = 0.01 * position.xy;
}  