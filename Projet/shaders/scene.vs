#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;

layout (location = 3) in vec4 a_instanceCol0;
layout (location = 4) in vec4 a_instanceCol1;
layout (location = 5) in vec4 a_instanceCol2;
layout (location = 6) in vec4 a_instanceCol3;
layout (location = 7) in vec4 a_instanceColor;
layout (location = 8) in float a_shadingMode;

out vec3 v_Normal;
out vec3 v_WorldPos;
out vec2 v_TexCoord;
out vec4 v_InstanceColor;
out float v_ShadingMode;

layout (std140) uniform CameraData {
    mat4 u_projection;
    mat4 u_view;
};

layout (std140) uniform ModelData {
    mat4 u_world;
    mat4 u_worldRot;
};

uniform int u_useInstancing;

void main(void)
{
    mat4 worldMat;
    mat4 rotMat;
    
    if (u_useInstancing == 1) {
        worldMat = mat4(a_instanceCol0, a_instanceCol1, a_instanceCol2, a_instanceCol3);
        rotMat = worldMat;
        rotMat[3] = vec4(0.0, 0.0, 0.0, 1.0);
        v_InstanceColor = a_instanceColor;
        v_ShadingMode = a_shadingMode;
    } else {
        worldMat = u_world;
        rotMat = u_worldRot;
        v_InstanceColor = vec4(1.0);
        v_ShadingMode = 2.0;
    }

    vec4 worldPos = worldMat * vec4(a_position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    v_Normal = (rotMat * vec4(a_normal, 0.0)).xyz;
    v_TexCoord = a_texcoord;
    
    gl_Position = u_projection * u_view * worldPos;
}
