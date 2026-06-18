#version 120
varying vec3 v_Normal;
varying vec3 v_Position;

uniform vec3 u_cameraPos;
uniform samplerCube u_skybox;
uniform int u_mode; // 0 = reflection, 1 = refraction

void main()
{
    // Vecteur de vue (de la camera vers le fragment)
    vec3 I = normalize(v_Position - u_cameraPos);
    vec3 N = normalize(v_Normal);
    
    vec3 R;
    if (u_mode == 0) {
        // Mode Réflexion (ex: Chrome, Miroir)
        R = reflect(I, N);
    } else {
        // Mode Réfraction (air -> verre, ratio ~ 1.00 / 1.52 = 0.658)
        float ratio = 1.00 / 1.52;
        R = refract(I, N, ratio);
    }
    
    // On échantillonne la cubemap avec le vecteur réfléchi ou réfracté
    gl_FragColor = textureCube(u_skybox, R);
}
