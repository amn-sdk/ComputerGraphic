#version 330 core

in vec3 v_TexCoords;
out vec4 FragColor;

uniform samplerCube u_skybox;

void main()
{
    FragColor = vec4(pow(texture(u_skybox, v_TexCoords).rgb, vec3(2.2)), 1.0);
}
