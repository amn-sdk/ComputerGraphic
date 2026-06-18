#version 330 core

in vec2 v_TexCoords;
out vec4 FragColor;

uniform sampler2D u_screenTexture;
uniform int u_postProcessMode;

void main()
{
    vec3 color = texture(u_screenTexture, v_TexCoords).rgb;

    if (u_postProcessMode == 1) {
        float gray = dot(color, vec3(0.2125, 0.7154, 0.072));
        color = vec3(gray);
    } else if (u_postProcessMode == 2) {
        color = vec3(
            dot(color, vec3(0.393, 0.769, 0.189)),
            dot(color, vec3(0.349, 0.686, 0.168)),
            dot(color, vec3(0.272, 0.534, 0.131))
        );
    } else if (u_postProcessMode == 3) {
        color = vec3(1.0) - color;
    }

    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
