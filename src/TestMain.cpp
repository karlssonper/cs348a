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
#include <vector>

using namespace std;

int width = 800;
int height = 600;

Vector3 p0(-9,-9, 0);
Vector3 p1(-8, -8, 10);
Vector3 p2(-7, -7, 7);
Vector3 p3(-6, -6, 5);
Vector3 p4(-5, -5, 7);
Vector3 p5(-5, 0, -2);
Vector3 p6(0, 0, 1);
Vector3 p7(3, 3, 3);
Vector3 p8(-4,-4,-4);

vector<Vector3> controlPoints;

float cameraT = 0.f;
float cameraInc = 0.05;
Vector3 cameraPos = p1;
Vector3 cameraDir = p2;

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


    BezierCurve::renderCurves(controlPoints, 1000, 3,f);
    BezierCurve::renderCtrlPts(controlPoints, 0.2);
    BezierCurve::renderCtrlPoly(controlPoints, 3.f);

    
}

void updateCamera() {
    cameraPos = BezierCurve::evaluate(p1, p2, p3, cameraT);
    cameraDir = BezierCurve::evaluate(p1, p2, p3, cameraT+cameraInc);
}
    
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0, -30, 20, 0, 0, 0, 0, 0, 1);

    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_TRIANGLES);
        glVertex3f(-10, -10, 0);
        glVertex3f(10, -10, 0);
        glVertex3f(-10, 10, 0);
        glVertex3f(-10, 10, 0);
        glVertex3f(10, -10, 0);
        glVertex3f(10, 10, 0);
    glEnd();

    /*
    gluLookAt(cameraPos.x,
              cameraPos.y,
              cameraPos.z,
              cameraDir.x,
              cameraDir.y,
              cameraDir.z,
              0, 1, 0);
    */

    //std::cout << "cameraPos: " << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
    //std::cout << "cameraDir: " << cameraDir.x << " " << cameraDir.y << " " << cameraDir.z << std::endl;
    

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

void keyPressed (unsigned char key, int x, int y) {  
    switch (key) {
    case 'w':
        cameraT += cameraInc;
        updateCamera();
        glutPostRedisplay();
        glutSwapBuffers();
        break;
    }
}

int main(int argc, char **argv)
{

    controlPoints.push_back(p0);
    controlPoints.push_back(p1);
    controlPoints.push_back(p2);
    controlPoints.push_back(p3);
    controlPoints.push_back(p4);
    controlPoints.push_back(p5);
    controlPoints.push_back(p6);
    controlPoints.push_back(p7);
    controlPoints.push_back(p8);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("cs348a project");
    initGL();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyPressed);
    glutMainLoop();
    return 0;
}
