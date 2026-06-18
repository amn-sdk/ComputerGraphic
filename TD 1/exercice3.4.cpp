#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <math.h>

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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); 
    DessinerTriangle();
    glPushMatrix();
        float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        float angleRad = (50.0f * time) * M_PI / 180.0f;
        float c = cos(angleRad);
        float s = sin(angleRad);
        float matRot[16] = {
             c,   s, 0.0f, 0.0f,
            -s,   c, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(matRot);
        float matScale[16] = {
          0.8f, 0.0f, 0.0f, 0.0f,
          0.0f, 0.8f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(matScale);
        float matTrans[16] = {
          1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.8f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(matTrans);
        DessinerTriangle();
    glPopMatrix();
    glPopMatrix(); 
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
    glutCreateWindow("TD 1 - Exercice 3.4 : Matrices Custom");
    glutDisplayFunc(Display);
    glutIdleFunc(Display);
    glutMainLoop();
    return 0;
}
