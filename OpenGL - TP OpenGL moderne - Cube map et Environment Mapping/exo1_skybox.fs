#version 120
varying vec3 v_TexCoords;
uniform samplerCube u_skybox;
void main()
{
    // On utilise textureCube (compatible OpenGL 2.x / GLSL 1.20)
    gl_FragColor = textureCube(u_skybox, v_TexCoords);
}
