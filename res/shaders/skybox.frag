#version 150

in vec3 v_tex_coords;

out vec4 color;

uniform samplerCube skybox;

void main()
{    
    color = texture(skybox, v_tex_coords);
}