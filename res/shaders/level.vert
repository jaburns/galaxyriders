#version 150

in vec2 position;
in float vdepth;

out float v_vdepth;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
    v_vdepth = vdepth;
}
