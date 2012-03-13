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

int miniWidth = 200;
int miniHeight = 150;

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

bool bDrawTerrain = true;
bool bDrawTours = true;
bool bDrawCurve = true;
bool bDrawControlPoints = true;
bool bDrawControlPolygon = false;

void reshape(int x, int y);

void drawTriangles()
{
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

void drawMinimap()
{
  glEnable(GL_LIGHTING);
  glScissor(0,0,miniWidth,miniHeight);
  glEnable(GL_SCISSOR_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, miniWidth, miniHeight);
  glOrtho(0.0, 10000.0, 0.0, 10000.0, 1.0, 10000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(5000.0, 5000.0, 5000.0,
	    5000.0, 5000.0, 0.0,
	    0.0, 1.0, 0.0);
  //  camera->lookThrough();
  drawTriangles();
  reshape(width,height);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_LIGHTING);
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
    /*glBegin(GL_LINES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3d(100000.f, 0.f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3d(0.f, 100000.f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.f, 0.f);
        glVertex3d(0.f, 0.f, 100000.f);
    glEnd();*/
    glEnable(GL_LIGHTING);

    GLfloat lightPos[] = { 0.0, -1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


    if(bDrawTerrain)
        drawTriangles();
    glDisable(GL_LIGHTING);
    if (bDrawTours)
        drawTour(&tour);

    if(bDrawCurve)
        BezierCurve::renderCurves(controlPoints,5000,3);

    if (bDrawControlPoints)
        BezierCurve::renderCtrlPts(controlPoints, 60);

    if (bDrawControlPolygon)
        BezierCurve::renderCtrlPoly(controlPoints,3);

    drawMinimap();
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

void printInfo()
{
    std::cout << std::endl <<
         "  ---------------------------------------------------------\n"
         "  | Stanford cs348a - Geometric Modeling                  |\n"
         "  |                  Final Project                        |\n"
         "  |       by Per Karlsson  - perk@stanford.edu            |\n"
         "  |          Victor Sand   - vsand@stanford.edu           |\n"
         "  |          Steven Lesser - sklesser@stanford.edu        |\n"
         "  |                                                       |\n"
         "  |    Mouse       - Rotate Camera view                   |\n"
         "  |    Press 'w/s' - Walk forward/backwards               |\n"
         "  |    Press 'a/d' - Strafe left/right                    |\n"
         "  |                                                       |\n"
         "  |    Press '1' - Toggle Terrain                         |\n"
         "  |    Press '2' - Toggle Sights                          |\n"
         "  |    Press '3' - Toggle Sight Path                      |\n"
         "  |    Press '4' - Toggle Control Points                  |\n"
         "  |    Press '5' - Toggle Control Polygon                 |\n"
         "  |                                                       |\n"
         "  |    Press 'esc' - Quit                                 |\n"
         "  ---------------------------------------------------------"
        << std::endl;
}

void keyPressed (unsigned char key, int x, int y) {  
    switch (key) {
    case '1':
        bDrawTerrain = !bDrawTerrain;
        break;
    case '2':
        bDrawTours = !bDrawTours;
        break;

    case '3':
        bDrawCurve = !bDrawCurve;
        break;
    case '4':
        bDrawControlPoints = !bDrawControlPoints;
        break;
    case '5':
        bDrawControlPolygon = !bDrawControlPolygon;
        break;
    case 'w':
        camera->walkForward(1000.f);
        break;
    case 's':
        camera->walkBackwards(1000.f);
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
    tour.resize(3);
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
    printInfo();
    glutMainLoop();
    return 0;
}
