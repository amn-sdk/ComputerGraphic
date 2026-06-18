#version 120
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

uniform mat4 u_projection;
uniform mat4 u_world;
uniform mat4 u_view;

varying vec3 v_Normal;
varying vec3 v_WorldPos;
varying vec2 v_TexCoord;

void main(void)
{
    gl_Position=u_projection*u_view*u_world*vec4(a_position,1.0);
    v_WorldPos=(u_world*vec4(a_position,1.0)).xyz;
    v_Normal=mat3(u_world)*a_normal;
    v_TexCoord=a_texcoord;
}
