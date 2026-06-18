#version 120
attribute vec3 a_position;
attribute vec3 a_normal;

varying vec3 v_Normal;
varying vec3 v_Position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_world;

void main()
{
    // Calcul de la position dans l'espace monde
    vec4 worldPos = u_world * vec4(a_position, 1.0);
    v_Position = worldPos.xyz;

    // Calcul de la normale dans l'espace monde (sans scaling non-uniforme)
    v_Normal = mat3(u_world) * a_normal;

    // Position finale projetee
    gl_Position = u_projection * u_view * worldPos;
}
