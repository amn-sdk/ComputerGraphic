#version 120
attribute vec3 a_position;
varying vec3 v_TexCoords;
uniform mat4 u_projection;
uniform mat4 u_view; // matrice de vue SANS translation (3x3 -> 4x4)
void main()
{
    v_TexCoords = a_position;
    // On met pos.xyww pour que la depth soit toujours 1.0 (fond)
    vec4 pos = u_projection * u_view * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
}
