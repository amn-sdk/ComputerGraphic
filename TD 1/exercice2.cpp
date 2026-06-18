#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

// Dimensions de la fenêtre (pour calculer les viewports)
int windowWidth = 960;
int windowHeight = 540;

// Fonction appelée quand la fenêtre est redimensionnée
void Reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
}

void Render()
{

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int halfW = windowWidth / 2;
    int halfH = windowHeight / 2;

    glViewport(0, halfH, halfW, halfH);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, halfH, halfW, halfH);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST); 
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex2f( 0.0f,  0.5f);
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex2f( 0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f); 
        glVertex2f(-0.5f, -0.5f);
    glEnd();

    // VIEWPORT EN HAUt À DROite
    glViewport(halfW, halfH, halfW, halfH);
    glEnable(GL_SCISSOR_TEST);
    glScissor(halfW, halfH, halfW, halfH);
    glClearColor(0.4f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    //Triangle Strip (GL_TRIANGLE_STRIP)
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(-0.6f,  0.5f);
        glColor3f(0.0f, 1.0f, 1.0f); glVertex2f(-0.6f, -0.5f);
        glColor3f(1.0f, 0.0f, 1.0f); glVertex2f( 0.0f,  0.5f);
        glColor3f(1.0f, 1.0f, 1.0f); glVertex2f( 0.6f, -0.5f);
    glEnd();

    // VIEWPORT EN bas à gauche
    glViewport(0, 0, halfW, halfH);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, halfW, halfH);
    glClearColor(0.0f, 0.4f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    
    // ligne épaisse
    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f); 
    glBegin(GL_LINES);
        glVertex2f(-0.8f, -0.8f);
        glVertex2f( 0.8f,  0.8f);
    glEnd();
    glLineWidth(1.0f); 

    // VIEWPORT BAS À DROITE
    glViewport(halfW, 0, halfW, halfH);
    glEnable(GL_SCISSOR_TEST);
    glScissor(halfW, 0, halfW, halfH);
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glPointSize(10.0f);
    glColor3f(1.0f, 0.0f, 1.0f); 
    glBegin(GL_POINTS);
        glVertex2f( 0.0f,  0.5f);
        glVertex2f( 0.5f, -0.5f);
        glVertex2f(-0.5f, -0.5f);
    glEnd();
    glPointSize(1.0f); 
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
    glutCreateWindow("TD 1 Exercice 2.1 : Viewports");
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape); 
    glutMainLoop();
    return 0;
}
