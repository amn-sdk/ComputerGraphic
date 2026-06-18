#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

void Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f( 0.0f,  0.5f); 
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f( -0.5f, -0.5f);  
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.5f, -0.5f);
    glEnd();
}

void Display()
{
    Render();
    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(960, 540);
    glutCreateWindow("TD 1 - Exercice 1.2 : Face Culling");
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}
