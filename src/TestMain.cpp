extern "C" {
    #include <triangulation.h>
    #include <planeSweep.h>
    #include <delaunay.h>
    #include <angle.h>
}

#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include "BezierCurve.h"
#include "MathEngine.h"

using namespace std;

int width = 800;
int height = 600;

void initGL() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(60.f, width/height, 0.1f, 100000.f);
    glMatrixMode(GL_MODELVIEW);
}

void drawBezier() {
    Vector3 p1(0.f, 0.f, 0.f);
    Vector3 p2(0.f, 1.f, 0.f);
    Vector3 p3(1.f, 1.f, 0.f);

    glColor3f(1.f, 0.f, 0.f);
    glPushMatrix(); glTranslatef(p1.x, p1.y, p1.z);
    glutSolidSphere(10, 10, 10);
    glPopMatrix();
    glPushMatrix(); glTranslatef(p2.x, p2.y, p2.z);
    glutSolidSphere(10, 10, 10);
    glPopMatrix();
    glPushMatrix(); glTranslatef(p2.x, p2.y, p2.z);
    glutSolidSphere(10, 10, 10);
    glPopMatrix();

    int steps = 100;

    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_POINTS);
    for (int i=0; i<steps; ++i) {
        float t = (float)i/(float)steps;
        Vector3 p = BezierCurve::evaluate(p1, p2, p3, t);
        glVertex3f(p.x, p.y, p.z);
    }   
    glEnd();
}
    
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
   
    drawBezier();

    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}

void reshape(int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, x, y);
    gluPerspective(60.f, x/y, 0.1f, 100000.f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("cs348a project");
    initGL();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}
