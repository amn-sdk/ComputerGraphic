import os

output_path = "/Users/aminesaddik/Documents/ESIEE/E4/S2/Computer graphique/OpenGL - TP OpenGL moderne - Cube map et Environment Mapping/complete_conversation_history.md"

if os.path.exists(output_path):
    with open(output_path, "r", encoding="utf-8") as f:
        content = f.read()
    
    # Summary of the truncated steps (0-335)
    intro_summary = """# Historique Complet de la Conversation
Ce fichier contient l'historique complet de vos échanges avec l'assistant IA Antigravity pour ce projet.

> [!NOTE]
> **Note sur les étapes 0 à 335 (Précédemment purgées par le système) :**
> L'IDE Antigravity applique une limite maximale à la taille de l'historique actif pour optimiser les performances. Les 335 premières étapes de notre conversation (datant du début du projet en mars 2026) ont vu leur contenu textuel brut purgé par le système.
> 
> Voici un résumé de ce qui a été réalisé et validé au cours de ces premières étapes (TD 1 et TD 2) :
> * **TD 1 - Primitives et Attributs (Ex 1.1 & 1.2) :** Implémentation du triangle coloré, observation de la rastérisation et de l'interpolation linéaire. Activation du Face Culling (`GL_CULL_FACE`) et correction de l'ordre d'affichage des sommets.
> * **TD 1 - Fenêtrage (Ex 2.1) :** Découpage de l'écran en 4 viewports distincts à l'aide de `glViewport()`, `glScissor()` et `glClear()`.
> * **TD 1 - Transformations (Ex 3.1 à 3.4) :** Combinaison de translations, rotations et homothéties. Remplacement des raccourcis OpenGL par une construction manuelle de matrices 4x4 personnalisées pour animer le triangle.
> * **TD 1 - Projections (Ex 4.1) :** Passage d'un système de coordonnées abstrait à un système en pixels réels (`gluOrtho2D`), puis mise en place de la perspective 3D avec point de fuite (`gluPerspective`).
> * **TD 1 - Caméra (Ex 5.1) :** Gestion de la caméra virtuelle et implémentation du positionnement via les transformations inverses.
>
> Les étapes détaillées ci-dessous reprennent à partir du nettoyage final du TD 1 et se poursuivent sur les TP suivants.

---

"""
    
    # Replace the old title line with our intro
    new_content = content.replace("# Historique Complet de la Conversation\nCe fichier contient l'historique complet de vos échanges avec l'assistant IA Antigravity pour ce projet.\n\n---\n\n", intro_summary)
    
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(new_content)
    print("Introduction de résumé insérée avec succès.")
