#version 120
attribute vec3 a_position;
attribute vec3 a_normal;
uniform mat4 u_mvp;
uniform mat4 u_worldRot;
varying vec3 v_Normal;
void main(void)
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_Normal = (u_worldRot * vec4(a_normal, 0.0)).xyz;
}
