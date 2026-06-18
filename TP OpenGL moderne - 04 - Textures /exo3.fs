#version 120
varying vec2 v_texcoords;
uniform sampler2D u_sampler0;  
uniform sampler2D u_sampler1;  
uniform int u_mode;
void main(void)
{
    vec4 color0 = texture2D(u_sampler0, v_texcoords);
    vec4 color1 = texture2D(u_sampler1, v_texcoords);
    vec4 result;
    if (u_mode == 0)
    {
        result = color0 + color1;
    }
    else if (u_mode == 1)
    {
        result = color0 * color1;
    }
    else
    {
        result = mix(color0, color1, 0.5);
    }
    gl_FragColor = result;
}
