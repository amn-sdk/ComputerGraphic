#version 330 core

layout (location = 0) in vec3 a_position;
out vec3 v_TexCoords;

layout (std140) uniform CameraData {
    mat4 u_projection;
    mat4 u_view;
};

void main()
{
    v_TexCoords = a_position;
    
    mat4 viewRot = mat4(mat3(u_view));
    vec4 pos = u_projection * viewRot * vec4(a_position, 1.0);
    
    gl_Position = pos;
}
