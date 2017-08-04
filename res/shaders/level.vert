#version 150

in vec2 position;
in vec2 surface_pos;

out vec4 v_world_pos;
out vec2 v_surface_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    v_world_pos = model * vec4(position, 0.0, 1.0);
    gl_Position = projection * view * v_world_pos;
    v_surface_pos = surface_pos;
}
