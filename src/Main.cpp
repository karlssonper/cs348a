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
#include "SightPath.h"
#include "BezierCurve.h"

using namespace std;

int width = 800;
int height = 600;

vector<int> x;
vector<int> y;
vector<int> z;
vector<Vector3> tour;
vector<Vector3> controlPoints;
triangleList *tl;
char *g;
int n, tn;
Camera* camera;
int mouseX, mouseY;

Terrain *terrain;
SightPath * sightPath;

void drawTriangles(triangleList *tl,
                   int *x,
                   int *y,
                   int *z) {

  glColor3f(1.f, 1.f, 1.f);
  terrain->renderTriangles();
}

void drawTour(vector<Vector3> *tour) {
  glColor3f(1.f, 0.f, 0.f);
  for (int i=0; i<tour->size(); ++i) {
    glPushMatrix();
    glTranslatef(tour->at(i).x, tour->at(i).y, tour->at(i).z);
    glutSolidSphere(50, 50, 50);
    glPopMatrix();
  }
}


void parseTour(string filename,
               vector<Vector3> *tour) {
    float xtemp, ytemp, ztemp;
    ifstream infile;
    infile.open(filename.c_str());
    if (infile.is_open()) {
        while (!infile.eof()) {
            infile >> xtemp;
            infile >> ytemp;
            infile >> ztemp;
            if (infile.eof()) break;
            tour->push_back(Vector3(xtemp,ytemp,ztemp));
        }
    } else {
        cout << filename << " could not be opened" << endl;
    }
    infile.close();  
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
    BezierCurve::renderCurves(controlPoints,1000);
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

void mouseFunc(int x,int y)
{
    int dx = x - mouseX;
    int dy = y - mouseY;
    mouseX = x;
    mouseY = y;
    camera->yawInc(1.6*dx);
    camera->pitchInc(1.6*dy);
}

void mouseMoveFunc(int x,int y)
{
    mouseX = x;
    mouseY = y;
}

int main(int argc, char **argv)
{
  /*parsePoints("../data/hw4.heights", &x, &y, &z, n);
    cout << "Parsed points, found " << n << " points" << endl;
    copyCoordinatesToGraph(n, &x[0], &y[0], &z[0], 1, &g);
    planeSweep(g);
    delaunay1(g);
    copyGraphToListOfTriangles(g, &tl);
*/
    parseTour("../data/hw4.tour", &tour);
    cout << "Parsed tour, found " << tour.size()/3 << " sights" << endl;

    terrain = new Terrain("../src/sample.mesh3","../src/sample.triangles3");
    printf("terrain loaded\n");
    sightPath = new SightPath(terrain, tour);
    printf("sight path loaded\n");
    sightPath->createConstraintTangents();
    printf("constraint trangents made\n");
    sightPath->createControlPoints();
    printf("control points created\n");
    controlPoints = sightPath->controlPoints();
    terrain->print();
    printf("everything made\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("cs348a project");
    initGL();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyPressed);
    glutMotionFunc(mouseFunc);
    glutPassiveMotionFunc(mouseMoveFunc);

    glutMainLoop();
    return 0;
}
