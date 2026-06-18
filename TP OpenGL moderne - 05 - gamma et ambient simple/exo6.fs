#version 120
varying vec3 v_Normal;
varying vec3 v_WorldPos;
varying vec2 v_TexCoord;
struct Light
{
    vec3 direction;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
};
struct Material
{
    vec3  ambientColor;
    vec3  specularColor;
    float shininess;
};
uniform Light      u_light;
uniform Material   u_material;
uniform vec3       u_cameraPos;
uniform sampler2D  u_texture;
const float GAMMA     = 2.2;
const float INV_GAMMA = 1.0 / 2.2;  
vec3 ambient()
{
    return u_material.ambientColor * u_light.ambientColor;
}
vec3 diffuse(vec3 N, vec3 L, vec3 Kd)
{
    float NdotL = max(dot(N, L), 0.0);
    return NdotL * u_light.diffuseColor * Kd;
}
vec3 specularBlinn(vec3 N, vec3 L, vec3 V)
{
    if (dot(N, L) <= 0.0) return vec3(0.0);
    vec3  H     = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float spec  = pow(NdotH, u_material.shininess);
    return spec * u_light.specularColor * u_material.specularColor;
}
void main()
{
    vec3 N=normalize(v_Normal);
    vec3 L=normalize(-u_light.direction);
    vec3 V=normalize(u_cameraPos-v_WorldPos);
    vec3 KdSRGB=texture2D(u_texture, v_TexCoord).rgb;
    vec3 KdLineaire=pow(KdSRGB,vec3(GAMMA));   
    vec3 colorAmbient= ambient();
    vec3 colorDiffuse= diffuse(N,L,KdLineaire);
    vec3 colorSpecular= specularBlinn(N,L,V);
    vec3 finalColorLineaire= colorAmbient+colorDiffuse+colorSpecular;
    vec3 finalColorSRGB= pow(finalColorLineaire,vec3(INV_GAMMA)); 
    gl_FragColor=vec4(finalColorSRGB,1.0);
}
