#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

void Render()
{
    // 1. Définir la couleur de "nettoyage" (R, G, B, A)
    // Ici : jaune (Rouge=1, Vert=1, Bleu=0, Opacité=1)
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

    // 2. Nettoyer l'écran avec la couleur définie au-dessus
    glClear(GL_COLOR_BUFFER_BIT);

    // 3. Dessiner un triangle
    glBegin(GL_TRIANGLES);
        // Coordonnées (x, y) entre -1.0 et 1.0
        glVertex2f( 0.0f,  0.5f);  // Haut milieu
        glVertex2f( 0.5f, -0.5f);  // Bas droite
        glVertex2f(-0.5f, -0.5f);  // Bas gauche
    glEnd();
}

// La 'callback' (fonction de rappel) exécutée par glutDisplayFunc()
void Display()
{
    Render();
    glutSwapBuffers(); // Échange les buffers (le back-buffer devient visible)
}

int main(int argc, char** argv)
{
    // Initialisation de GLUT
    glutInit(&argc, argv);

    // Mode d'affichage : un seul buffer (GLUT_SINGLE)
    glutInitDisplayMode(GLUT_DOUBLE);

    // Taille de la fenêtre
    glutInitWindowSize(960, 540);

    // Création de la fenêtre avec un titre
    glutCreateWindow("Triangle - Base");

    // On dit à GLUT quelle fonction appeler pour dessiner
    glutDisplayFunc(Display);

    // Boucle infinie qui gère les événements (clics, affichage, etc.)
    glutMainLoop();

    return 0;
}
