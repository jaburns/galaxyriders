#version 150

in vec3 position;

out vec3 v_position;
out vec2 v_tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 sprite_source;
uniform vec4 sprite_frame;

void main()
{
    v_position = vec3(model * vec4(position, 1.0f));
    gl_Position = projection * view * model * vec4(position, 1.0f);
    v_tex_coords = position.xy;
}  