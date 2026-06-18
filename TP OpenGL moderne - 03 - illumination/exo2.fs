#version 120
varying vec3 v_Normal;
varying vec2 v_TexCoord;
uniform vec3 u_lightDir;    
uniform vec3 u_lightColor;  
uniform sampler2D u_texture;
vec3 diffuse(vec3 N, vec3 L, vec3 Kd)
{
    float NdotL = max(dot(N, L), 0.0);
    return NdotL * u_lightColor * Kd;
}
void main()
{
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(-u_lightDir);
    vec3 Kd = texture2D(u_texture, v_TexCoord).rgb;
    vec3 color = diffuse(N, L, Kd);
    gl_FragColor = vec4(color, 1.0);
}
