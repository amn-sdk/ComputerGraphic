#version 120
varying vec3 v_Normal;
uniform vec3 u_lightDir;
uniform vec3 u_lightColor;
vec3 diffuse(vec3 N, vec3 L)
{
    float NdotL = max(dot(N, L), 0.0);
    return NdotL * u_lightColor;
}
void main()
{
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(-u_lightDir);
    vec3 color = diffuse(N, L);
    gl_FragColor = vec4(color, 1.0);
}
