#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

void Render()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f( 0.0f,  0.5f);  
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f( 0.5f, -0.5f);  
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(-0.5f, -0.5f);
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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(960, 540);
    glutCreateWindow("TD 1 - Exercice 1.1 : Couleurs");
    glutDisplayFunc(Display);
    glutMainLoop();

    return 0;
}
