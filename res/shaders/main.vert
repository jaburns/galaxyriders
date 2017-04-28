#version 150

in vec3 vPos;
in vec3 vNorm;

out vec2 v_tex_coords;
out vec3 v_normal;

uniform mat4 model;
uniform mat4 perspective;

void main()
{
   v_normal = transpose(inverse(mat3(model))) * vNorm;
   v_tex_coords = 0.01 * vPos.xy;
   gl_Position = perspective * model * vec4(vPos, 1.0);
}