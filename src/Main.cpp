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

/*void calculateNormal(float x1,  float y1,  float z1,
                     float x2,  float y2,  float z2, 
                     float x3,  float y3,  float z3,
                     float *nx, float *ny, float *nz) {
    float Qx,Qy,Qz,Px,Py,Pz, nxx, nyy, nzz, norm;
    Qx = x2-x1;
    Qy = y2-y1;
    Qz = z2-z1;
    Px = x3-x2;
    Py = y3-y2;
    Pz = z3-z2;
    nxx = Qy*Pz - Qz*Py;
    nyy = -Qx*Pz + Qz*Px;
    nzz = Qx*Py - Px*Qy;
    norm = sqrt(nxx*nxx+nyy*nyy+nzz*nzz);
    *nx = nxx / norm;
    *ny = nyy / norm;
    *nz = nzz / norm;
    }*/

void drawTriangles(triangleList *tl,
                   int *x,
                   int *y,
                   int *z) {

    glColor3f(1.f, 1.f, 1.f);

    /*    for (int i=0; i<tl->nofTriangles; ++i) {
        glBegin(GL_TRIANGLES);
        float nx, ny, nz;
        calculateNormal(x[tl->v[i][0]], y[tl->v[i][0]], z[tl->v[i][0]],
                        x[tl->v[i][1]], y[tl->v[i][1]], z[tl->v[i][1]],
                        x[tl->v[i][2]], y[tl->v[i][2]], z[tl->v[i][2]],
                        &nx, &ny, &nz);
        glNormal3f(nx,ny,nz);
        glVertex3f(x[tl->v[i][0]], y[tl->v[i][0]], z[tl->v[i][0]]);
        glVertex3f(x[tl->v[i][1]], y[tl->v[i][1]], z[tl->v[i][1]]);
        glVertex3f(x[tl->v[i][2]], y[tl->v[i][2]], z[tl->v[i][2]]);
        glEnd();
	}*/
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

void parsePoints(string filename,
                 vector<int> *x, 
                 vector<int> *y, 
                 vector<int> *z,
                 int &n) {
    n = 0;
    string temp;
    int xtemp, ytemp, ztemp;
    ifstream infile;
    infile.open(filename.c_str());
    if (infile.is_open()) {
        while (!infile.eof()) {
            infile >> temp;
            if (infile.eof()) break;
            infile >> xtemp;
            infile >> ytemp;
            infile >> ztemp;
            x->push_back(xtemp);
            y->push_back(ytemp);
            z->push_back(ztemp);
            n++;
            cout << xtemp << " " << ytemp << " " << ztemp << endl;
        }
    } else {
        cout << filename << " could not be opened" << endl;
    }
    infile.close();   
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

    //drawTerrain();
    drawTriangles(tl, &x[0], &y[0], &z[0]);

    glDisable(GL_LIGHTING);
    drawTour(&tour);
    glEnable(GL_LIGHTING);

    //glFlush();
    //glutPostRedisplay();
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
    sightPath = new SightPath(terrain, tour);
    sightPath->createConstraintTangents();
    sightPath->createControlPoints();
    controlPoints = sightPath->controlPoints();
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
    glutMotionFunc(mouseFunc);
    glutPassiveMotionFunc(mouseMoveFunc);

    glutMainLoop();
    return 0;
}
