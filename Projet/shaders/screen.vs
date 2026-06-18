#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_texcoords;
out vec2 v_TexCoords;

void main(void)
{
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_TexCoords = a_texcoords;
}
