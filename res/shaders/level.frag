#version 150

in float v_vdepth;

out vec4 color;

void main()
{
    color = vec4(0.416, 0.361, v_vdepth, 1.0);
}
