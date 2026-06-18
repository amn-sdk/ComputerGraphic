#version 120
varying vec3 v_Normal;
varying vec3 v_WorldPos;
varying vec2 v_TexCoord;
uniform vec3 u_lightDir;         
uniform vec3 u_lightDiffColor;   
uniform vec3 u_lightSpecColor;   
uniform vec3  u_Ks;              
uniform float u_shininess;       
uniform vec3 u_cameraPos;        
uniform sampler2D u_texture;
vec3 diffuse(vec3 N, vec3 L, vec3 Kd)
{
    float NdotL = max(dot(N, L), 0.0);
    return NdotL * u_lightDiffColor * Kd;
}
vec3 specular(vec3 N, vec3 L, vec3 V, vec3 Ks)
{
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    float spec = pow(RdotV, u_shininess);
    float NdotL = dot(N, L);
    if (NdotL <= 0.0)
        spec = 0.0;
    return spec * u_lightSpecColor * Ks;
}
void main()
{
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(-u_lightDir);
    vec3 V = normalize(u_cameraPos - v_WorldPos);
    vec3 Kd = texture2D(u_texture, v_TexCoord).rgb;
    vec3 diff = diffuse(N, L, Kd);
    vec3 spec = specular(N, L, V, u_Ks);
    vec3 color = diff + spec;
    gl_FragColor = vec4(color, 1.0);
}
