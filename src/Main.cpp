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
#include "Camera.h"
#include "terrain.h"

using namespace std;

int width = 800;
int height = 600;

vector<int> x;
vector<int> y;
vector<int> z;
vector<float> tour;
triangleList *tl;
char *g;
int n, tn;
Camera* camera;

Terrain *terrain;

void drawTriangles(triangleList *tl,
                   int *x,
                   int *y,
                   int *z) {

    glColor3f(1.f, 1.f, 1.f);
    terrain->renderTriangles();
}

void drawTour(vector<float> *tour) {
    glColor3f(1.f, 0.f, 0.f);
    for (int i=0; i<tour->size()/3; ++i) {
        glPushMatrix();
        glTranslatef(tour->at(3*i), tour->at(3*i+1), tour->at(3*i+2));
        glutSolidSphere(50, 50, 50);
        glPopMatrix();
    }
}

void initGL() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(60.f, width/height, 0.1f, 100000.f);
    glMatrixMode(GL_MODELVIEW);
    camera = new Camera(0.0f, 0.0f, 2000.f, 0.f, 0.f);
}
    
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    
    camera->lookThrough();
       
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3d(100000.f, 0.f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3d(0.f, 100000.f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3d(0.f, 0.f, 100000.f);
    glEnd();
    glEnable(GL_LIGHTING);

    GLfloat lightPos[] = { 0.0, -1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawTriangles(tl, &x[0], &y[0], &z[0]);

    glDisable(GL_LIGHTING);
    drawTour(&tour);
    glEnable(GL_LIGHTING);

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
    case 'r':
        camera->pitchInc(-3.f);
        break;
    case 'f':
        camera->pitchInc(3.f);
        break;
    case 'q':
        camera->yawInc(-3.f);
        break;
    case 'e':
        camera->yawInc(3.f);
        break;
    case 'w':
        camera->walkForward(100.f);
        break;
    case 's':
        camera->walkBackwards(100.f);
        break;
    case 'a':
        camera->strafeLeft(100.f);
        break;
    case 'd':
        camera->strafeRight(100.f);
        break;
    }
}  

int main(int argc, char **argv)
{
  /*parsePoints("../data/hw4.heights", &x, &y, &z, n);
    cout << "Parsed points, found " << n << " points" << endl;
    copyCoordinatesToGraph(n, &x[0], &y[0], &z[0], 1, &g);
    planeSweep(g);
    delaunay1(g);
    copyGraphToListOfTriangles(g, &tl);

    parseTour("../data/hw4.tour", &tour);
    cout << "Parsed tour, found " << tour.size()/3 << " sights" << endl;*/

    terrain = new Terrain("../src/sample.mesh3","../src/sample.triangles3");
    terrain->print();
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
