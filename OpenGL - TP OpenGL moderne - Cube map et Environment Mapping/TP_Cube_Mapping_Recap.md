# Récapitulatif du TP : Cube Mapping et Environment Mapping
**Auteurs :** Kevin SUTIA / Amine SADDIK

Ce document résume l'ensemble des travaux, codes, shaders et commandes de compilation réalisés au cours de ce TP pour un accès et une consultation faciles directement depuis votre IDE.

---

## 1. Objectifs du TP
L'objectif de ce TP était d'implémenter le **Cube Mapping** sous OpenGL moderne :
*   **Exercice 1 :** Création d'une Skybox (boîte d'environnement à l'infini).
*   **Exercice 2 :** Environment Mapping sur un objet central (Réflexion type miroir/chrome et Réfraction type verre).
*   **Exercice 3 (Test) :** Utilisation des textures de test (`test_px.png`, `test_nx.png`, etc.) sur le décor et sur un cube central pour valider l'orientation de chaque face de la cubemap.

---

## 2. Commandes de Compilation et Exécution

Toutes les sources se compilent sur macOS avec la chaîne d'outils Homebrew (`glfw` et `glew`).

### Compiler l'Exercice 1 (Skybox de Pise) :
```bash
clang++ exercice1.cpp common/GLShader.cpp -std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL -o exercice1
./exercice1
```

### Compiler l'Exercice 2 (Cube avec Réflexion/Réfraction - Pisa) :
```bash
clang++ exercice2.cpp common/GLShader.cpp -std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL -o exercice2
./exercice2
```

### Compiler l'Exercice 3 Test (Cube avec Réflexion/Réfraction - Textures Test) :
```bash
clang++ exercice3_test.cpp common/GLShader.cpp -std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL -o exercice3_test
./exercice3_test
```

---

## 3. Contrôles et Raccourcis
*   **Clic gauche + Glisser la souris :** Rotation orbitale de la caméra autour de la scène.
*   **Molette de la souris :** Zoom avant / Zoom arrière (ajustement de la distance de la caméra).
*   **Touche R :** Activer le mode **Réflexion** (l'objet se comporte comme un miroir/chrome).
*   **Touche F :** Activer le mode **Réfraction** (l'objet se comporte comme du verre avec réfraction air-verre ratio `1.00 / 1.52`).
*   **Touche Echap :** Quitter l'application proprement.

---

## 4. Les Fichiers Shaders

### 4.1. Shaders de la Skybox (Exercice 1)
#### Vertex Shader (`exo1_skybox.vs`) :
```glsl
#version 120
attribute vec3 a_position;
varying vec3 v_TexCoords;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    v_TexCoords = a_position;
    // L'astuce pos.xyww garantit que la skybox a une profondeur de Z = 1.0 (fond de la scene) après division perspective
    vec4 pos = u_projection * u_view * vec4(a_position, 1.0);
    gl_Position = pos.xyww;
}
```

#### Fragment Shader (`exo1_skybox.fs`) :
```glsl
#version 120
varying vec3 v_TexCoords;
uniform samplerCube u_skybox;

void main()
{
    gl_FragColor = textureCube(u_skybox, v_TexCoords);
}
```

### 4.2. Shaders de l'Objet Central (Exercice 2 & 3)
#### Vertex Shader (`exo2_envmap.vs`) :
```glsl
#version 120
attribute vec3 a_position;
attribute vec3 a_normal;

varying vec3 v_Normal;
varying vec3 v_Position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_world;

void main()
{
    vec4 worldPos = u_world * vec4(a_position, 1.0);
    v_Position = worldPos.xyz;
    v_Normal = mat3(u_world) * a_normal; // Transformation des normales dans le monde
    gl_Position = u_projection * u_view * worldPos;
}
```

#### Fragment Shader (`exo2_envmap.fs`) :
```glsl
#version 120
varying vec3 v_Normal;
varying vec3 v_Position;

uniform vec3 u_cameraPos;
uniform samplerCube u_skybox;
uniform int u_mode; // 0 = reflection, 1 = refraction

void main()
{
    vec3 I = normalize(v_Position - u_cameraPos);
    vec3 N = normalize(v_Normal);
    
    vec3 R;
    if (u_mode == 0) {
        R = reflect(I, N);
    } else {
        float ratio = 1.00 / 1.52; // Indice de refraction air/verre
        R = refract(I, N, ratio);
    }
    gl_FragColor = textureCube(u_skybox, R);
}
```

---

## 5. Détails Techniques & Corrections Clés

### Alignement des pixels (Exercice 3)
Lors de l'utilisation des textures de test (`test_px.png`, etc.), un bug d'affichage (bandes obliques colorées) a été résolu en configurant correctement l'alignement des lignes de pixels non multiples de 4 dans la mémoire :
```cpp
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
```
De plus, la fonction `stbi_load` a été forcée à retourner 3 canaux (`RGB`) afin d'éviter tout décalage avec des fichiers PNG contenant un canal alpha optionnel.
