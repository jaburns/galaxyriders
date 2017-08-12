#version 150

in vec2 position;
in vec3 surface_info;

out vec4 v_world_pos;
out vec3 v_surface_info;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    v_world_pos = model * vec4(position, 0.0, 1.0);
    gl_Position = projection * view * v_world_pos;
    v_surface_info = surface_info;
}
