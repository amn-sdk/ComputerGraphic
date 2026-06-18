#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

int windowWidth = 960;
int windowHeight = 540;

void Reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    
    glMatrixMode(GL_MODELVIEW);
}

void DessinerTriangle() {
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex2f( 0.0f,  0.5f);
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex2f( 0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f); 
        glVertex2f(-0.5f, -0.5f);
    glEnd();
}

void Render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glTranslatef(windowWidth / 2.0f, windowHeight / 2.0f, 0.0f);
    
    glScalef(200.0f, 200.0f, 1.0f);
    
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
    glutCreateWindow("TD 1 Exercice 4.1.a : gluOrtho2D en pixels");
    
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape); 
    
    glutMainLoop();
    return 0;
}
