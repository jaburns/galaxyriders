#version 150

in vec3 vPos;
in vec3 vNorm;

out vec2 v_tex_coords;
out vec3 v_normal;

uniform mat4 mv;
uniform mat4 mvp;

void main()
{
    v_normal = transpose(inverse(mat3(mv))) * vNorm;
    v_tex_coords = 0.01 * vPos.xy;
    gl_Position = mvp * vec4(vPos, 1.0);
}
