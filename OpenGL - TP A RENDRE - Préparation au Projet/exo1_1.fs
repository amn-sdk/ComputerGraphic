#version 120
varying vec3 v_Normal;
varying vec3 v_WorldPos;
varying vec2 v_TexCoord;
struct Light
{
    vec3 direction;
    vec3 diffuseColor;   
    vec3 specularColor;  
};
struct Material
{
    vec3  specularColor; 
    float shininess;
};
uniform Light u_light;
uniform Material u_material;
uniform vec3 u_cameraPos;
uniform sampler2D u_texture;  
uniform vec3 u_skyDirection; 
uniform vec3 u_skyColor;     
uniform vec3 u_groundColor;  
uniform vec3 u_Ia;           
vec3 ambientHemispheric(vec3 N)
{
    float NdotSky = dot(N, normalize(u_skyDirection));
    float HemisphereFactor = NdotSky * 0.5 + 0.5;
    vec3 ambientColor = mix(u_groundColor, u_skyColor, HemisphereFactor);
    return u_Ia * ambientColor;
}
vec3 diffuse(vec3 N, vec3 L, vec3 Kd)
{
    float NdotL = max(dot(N, L), 0.0);
    return NdotL * u_light.diffuseColor * Kd;
}
vec3 specularBlinn(vec3 N, vec3 L, vec3 V)
{
    if (dot(N, L) <= 0.0) return vec3(0.0);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    return pow(NdotH, u_material.shininess) * u_light.specularColor * u_material.specularColor;
}
void main()
{
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(-u_light.direction);
    vec3 V = normalize(u_cameraPos - v_WorldPos);
    vec3 KdSRGB = texture2D(u_texture, v_TexCoord).rgb;
    vec3 Kd = pow(KdSRGB, vec3(2.2)); 
    vec3 colorAmbient = ambientHemispheric(N);  
    vec3 colorDiffuse = diffuse(N, L, Kd);
    vec3 colorSpecular = specularBlinn(N, L, V);
    vec3 finalColor = colorAmbient + colorDiffuse + colorSpecular;
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    gl_FragColor = vec4(finalColor, 1.0);
}
