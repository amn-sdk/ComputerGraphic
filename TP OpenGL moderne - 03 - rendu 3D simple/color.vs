#version 120
attribute vec3 a_position;
uniform mat4 u_mvp;
varying vec3 v_color;
void main(void)
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_color = (a_position + 1.0) * 0.5;
}
