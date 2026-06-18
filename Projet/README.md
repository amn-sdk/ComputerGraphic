# Projet OpenGL - Rendu interactif

Projet final de computer graphics réalisé sur Mac par :

- Amine SADDIK
- Yassine Housny
- Kevin Sutia

Le but du projet est d'afficher une scène 3D composée de plusieurs objets, avec plusieurs types de matériaux et d'illuminations, puis de pouvoir naviguer dans la scène. Le projet reste volontairement dans les techniques vues en TP et en cours : shaders GLSL, VAO/VBO, matrices de transformation, textures, cubemap, FBO, UBO, instancing et modèle d'illumination de Phong.

## Lancement

Compilation :

```bash
./build.sh
```

Exécution :

```bash
./app
```

Le projet utilise OpenGL 3.3 Core Profile. Il a été développé et testé sur macOS.

## Contrôles

Les contrôles principaux utilisés pour la démonstration sont :

- `C` : basculer entre la caméra orbitale et la caméra libre.
- `T` : changer la cible de la caméra orbitale entre le dragon et certains objets de la scène.
- Clic gauche + déplacement de souris : faire tourner la caméra orbitale autour de la cible.
- Molette ou flèches `haut` / `bas` : zoomer ou dézoomer en mode orbital.
- `1` : rendu normal.
- `2` : post-traitement en niveaux de gris.
- `3` : post-traitement sépia.
- `4` : post-traitement inversion des couleurs.
- `I` : augmenter l'effet de contour lumineux de type Fresnel / back-lighting.
- `O` : diminuer cet effet.
- `Echap` : quitter l'application.

Remarque : les déplacements clavier de type `WASD` ou `ZQSD` ne sont pas utilisés dans la démonstration finale.

## Contenu de la scène

La scène contient plusieurs objets afin de montrer différentes techniques de rendu.

### Dragon

Le dragon est l'objet central de la scène. Il utilise un rendu de type environment mapping avec la cubemap de la scène. Cela donne un aspect métallique/réfléchissant : la couleur du dragon dépend beaucoup de l'environnement autour de lui.

Le dragon utilise aussi :

- une contribution spéculaire indirecte via la cubemap ;
- une approximation de Fresnel avec la formule de Schlick ;
- un effet de back-lighting, réglable avec `I` et `O`.

Quand on appuie plusieurs fois sur `I`, les contours et certaines zones brillantes du dragon deviennent plus visibles. C'est l'effet attendu : on augmente l'intensité du halo lumineux qui simule un éclairage venant de l'arrière ou du bord de l'objet.

### Pyramide verte

La pyramide verte montre un rendu simple de type Lambert. Elle sert d'exemple d'objet avec une couleur simple, sans texture et sans effet spéculaire fort.

Technique principale :

- couleur diffuse simple ;
- illumination directe Lambert.

### Pyramide bleue

La pyramide bleue montre une variante avec ambiante hémisphérique. Cette technique simule une partie de l'illumination indirecte en mélangeant une couleur de ciel et une couleur de sol selon l'orientation de la normale.

Technique principale :

- Lambert ;
- ambiante hémisphérique.

### Cube texturé

Le cube multicolore utilise une texture chargée depuis le matériau OBJ/MTL. Le fichier `pyramid.mtl` contient maintenant :

```mtl
map_Kd texture2.png
```

La texture est donc chargée par TinyOBJLoader via le matériau, puis utilisée par le shader.

Techniques principales :

- chargement OBJ avec matériau ;
- texture diffuse `map_Kd` ;
- correction gamma de la texture vers l'espace linéaire ;
- Phong / Blinn-Phong.

### Cube jaune

Le cube jaune montre un rendu Phong sans texture. Il permet de voir une surface colorée, avec diffuse et spéculaire, mais sans image appliquée dessus.

Techniques principales :

- couleur diffuse ;
- Phong / Blinn-Phong ;
- composante spéculaire.

### Cône réfléchissant

Le cône utilise un matériau plus réfléchissant. Il sert à montrer l'utilisation de la cubemap comme environment map sur un autre objet que le dragon.

Techniques principales :

- environment mapping ;
- composante spéculaire indirecte ;
- mélange diffuse / spéculaire avec Fresnel.

## Techniques OpenGL utilisées

### Chargement de modèles OBJ

Les objets `pyramid.obj`, `cube.obj` et `cone.obj` sont chargés avec TinyOBJLoader. Le loader récupère :

- les positions ;
- les normales ;
- les coordonnées de texture ;
- les matériaux `Ka`, `Kd`, `Ks`, `Ns` ;
- les textures déclarées avec `map_Kd`.

Le fichier `model.h` envoie ensuite les données de matériau au shader.

### Illumination directe

Le shader principal implémente une illumination directe de type Phong / Blinn-Phong :

- diffuse Lambert avec `max(dot(N, L), 0.0)` ;
- spéculaire Blinn-Phong avec le demi-vecteur `H` ;
- shininess venant du matériau OBJ/MTL.

### Illumination indirecte

Deux formes d'illumination indirecte sont utilisées :

- ambiante hémisphérique pour simuler une lumière diffuse venant du ciel et du sol ;
- environment mapping pour simuler la réflexion spéculaire de l'environnement.

### FBO et post-traitement

Le rendu principal est d'abord fait dans un FBO. Ensuite, la texture du FBO est dessinée sur un quad plein écran avec `screen.fs`.

Cela permet :

- d'appliquer les filtres `1`, `2`, `3`, `4` ;
- de faire la correction gamma finale avant l'affichage.

### Skybox et cubemap

La scène affiche une skybox chargée avec une cubemap. Cette même cubemap est réutilisée dans les shaders pour l'environment mapping du dragon et du cône.

### UBO

Les matrices de projection et de vue sont envoyées aux shaders avec un Uniform Buffer Object `CameraData`. La matrice monde de l'objet non instancié est aussi envoyée via un UBO `ModelData`.

### Instancing

Les objets OBJ simples sont affichés avec de l'instancing hardware. Les données propres à chaque instance contiennent :

- une matrice monde ;
- une couleur ;
- un mode de shading.

Cela permet d'afficher plusieurs objets avec un même mesh, mais avec des positions, couleurs et effets différents.

## Problèmes bloquants rencontrés

Nous avons rencontré plusieurs problèmes importants pendant la mise au point. Nous avons utilisé l'IA (GPT et gémini) comme aide au débogage pour identifier les causes, corriger le code et vérifier que les corrections respectaient bien les techniques vues en TP.

### 1. Bypass du FBO

Le rendu principal ne passait plus correctement par le FBO. Cela empêchait de valider la partie 1.d de l'énoncé et rendait les post-traitements inutilisables.

Correction apportée :

- restauration du rendu dans `g_FBO` ;
- ajout de la passe finale vers le backbuffer ;
- dessin du quad plein écran avec `g_ScreenShader` ;
- application des filtres et de la correction gamma dans `screen.fs`.

### 2. Erreurs OpenGL 0x501 / 0x502

L'application générait des erreurs OpenGL à l'initialisation. L'erreur `GL_INVALID_VALUE` empêchait l'allocation correcte de l'UBO de la caméra. Ensuite, des erreurs `GL_INVALID_OPERATION` apparaissaient en continu pendant le rendu.

Conséquence observée :

- écran presque uniforme bleu-vert ;
- pas de skybox visible ;
- pas de modèles 3D affichés correctement.

Correction apportée :

- nettoyage de l'initialisation OpenGL ;
- vérification des UBO ;
- vérification des bindings `CameraData` et `ModelData` ;
- ajout de contrôles d'erreurs OpenGL pendant l'initialisation.

### 3. VAO sur macOS

Le code contenait des directives utilisant les anciennes extensions Apple :

```cpp
glGenVertexArraysAPPLE
```

Sur un contexte OpenGL 3.3 Core Profile sous macOS, les VAO font déjà partie du standard OpenGL. Ces anciennes extensions créaient donc des conflits.

Correction apportée :

- suppression de l'utilisation forcée des extensions VAO Apple ;
- utilisation des fonctions standard `glGenVertexArrays`, `glBindVertexArray`, etc.

## Etat final

Le projet compile et affiche une scène complète avec :

- plusieurs objets 3D ;
- chargement OBJ avec TinyOBJLoader ;
- matériaux issus du fichier MTL ;
- texture issue de `map_Kd` ;
- Lambert ;
- Phong / Blinn-Phong ;
- illumination indirecte ;
- environment mapping ;
- skybox cubemap ;
- rendu hors écran avec FBO ;
- post-traitements ;
- gamma correction ;
- UBO ;
- instancing ;
- navigation orbitale ;
- effet Fresnel / back-lighting réglable.

Les options ImGui et Compute Shader n'ont pas été ajoutées, car l'énoncé demandait une ou plusieurs options, et le projet en implémente déjà plusieurs : post-traitement, instancing, skybox cubemap, Fresnel / back-lighting et Fresnel de Schlick.
