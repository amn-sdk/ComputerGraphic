#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

int windowWidth = 960;
int windowHeight = 540;

void Reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void DessinerTriangle() {
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( 0.0f,  0.0f,  0.5f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f( 0.5f,  0.0f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.5f,  0.0f, -0.5f);
    glEnd();
}

void Render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(
        0.0, 3.0, 0.0,  
        0.0, 0.0, 0.0, 
        0.0, 0.0, -1.0   
    );

    DessinerTriangle();
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
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("TD 1 Exercice 5.1 : Camera gluLookAt");
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutMainLoop();
    return 0;
}
