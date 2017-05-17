#version 150

in vec2 v_normal;
in vec2 v_position;

out vec4 color;

uniform vec3 camera_pos;
uniform samplerCube skybox;
// uniform sampler2D tex;

// https://learnopengl.com/#!Advanced-OpenGL/Cubemaps
// https://learnopengl.com/code_viewer.php?code=advanced/cubemaps_skybox

void main()
{
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(v_position - camera_pos);
    vec3 R = refract(I, normalize(v_normal), ratio);
    color = texture(skybox, R);
}
