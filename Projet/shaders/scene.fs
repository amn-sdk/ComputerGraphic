#version 330 core

in vec3 v_Normal;
in vec3 v_WorldPos;
in vec2 v_TexCoord;
in vec4 v_InstanceColor;
in float v_ShadingMode;

out vec4 FragColor;

struct Light
{
    vec3 direction;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct Material
{
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
    int hasTexture;
};

uniform Light    u_light;
uniform Material u_material;
uniform vec3     u_cameraPos;
uniform sampler2D u_texture;
uniform samplerCube u_skybox;

uniform vec3 u_skyDirection;
uniform vec3 u_skyColor;
uniform vec3 u_groundColor;
uniform vec3 u_Ia;

uniform float u_backLightStrength;

vec3 ambientHemispheric(vec3 N)
{
    float NdotSky = dot(N, normalize(u_skyDirection));
    float factor = NdotSky * 0.5 + 0.5;
    return u_Ia * mix(u_groundColor, u_skyColor, factor);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(-u_light.direction);
    vec3 V = normalize(u_cameraPos - v_WorldPos);

    int mode = int(v_ShadingMode + 0.5);

    vec3 albedo;
    if (mode == 1) {
        vec4 texSample = texture(u_texture, v_TexCoord);
        albedo = pow(texSample.rgb, vec3(2.2));
    } else {
        albedo = u_material.diffuseColor;
    }

    albedo *= v_InstanceColor.rgb;
    float NdotL = max(dot(N, L), 0.0);
    vec3 baseDirectDiffuse = NdotL * u_light.diffuseColor * albedo;

    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float specPower = pow(NdotH, u_material.shininess);

    float cosThetaD = max(dot(H, V), 0.0);
    vec3 F0 = u_material.specularColor;
    vec3 F_direct = fresnelSchlick(cosThetaD, F0);

    float normFactor = (u_material.shininess + 8.0) / 8.0;
    vec3 directSpecularNorm = (NdotL > 0.0) ? (specPower * NdotL * normFactor * u_light.specularColor) : vec3(0.0);

    vec3 directSpecular = F_direct * directSpecularNorm;
    vec3 directDiffuse = (vec3(1.0) - F_direct) * baseDirectDiffuse;

    float backLight = pow(max(1.0 - dot(N, V), 0.0), 3.0);
    vec3 backLighting = backLight * u_light.diffuseColor * u_backLightStrength;

    vec3 finalColor = vec3(0.0);

    if (mode == 0) {
        vec3 ambient = u_Ia * albedo;
        finalColor = ambient + baseDirectDiffuse;
    }
    else if (mode == 1) {
        vec3 ambient = ambientHemispheric(N) * albedo;
        finalColor = ambient + directDiffuse + directSpecular + backLighting;
    }
    else if (mode == 2) {
        vec3 R = reflect(-V, N);
        vec3 envSpecularColor = pow(texture(u_skybox, R).rgb, vec3(2.2));
        
        vec3 envDiffuseColor = pow(texture(u_skybox, N).rgb, vec3(2.2));
        
        float cosTheta = max(dot(N, V), 0.0);
        vec3 F = fresnelSchlick(cosTheta, F0);
        
        vec3 indirectSpecular = F * envSpecularColor;
        vec3 indirectDiffuse = (vec3(1.0) - F) * envDiffuseColor * albedo;
        
        finalColor = indirectDiffuse + indirectSpecular + directDiffuse + directSpecular + backLighting;
    }
    else if (mode == 3) {
        vec3 ambient = ambientHemispheric(N) * albedo;
        finalColor = ambient + baseDirectDiffuse;
    }
    else if (mode == 4) {
        vec3 ambient = ambientHemispheric(N) * albedo;
        finalColor = ambient + directDiffuse + directSpecular + backLighting;
    }

    FragColor = vec4(finalColor, 1.0);
}
