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

using namespace std;

int width = 800;
int height = 600;

vector<int> x;
vector<int> y;
vector<int> z;
triangleList *tl;
char *g;
int n;

void calculateNormal(float x1,  float y1,  float z1,
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
}

void drawTriangles(triangleList *tl,
                   int *x,
                   int *y,
                   int *z) {

    glColor3f(1.f, 1.f, 1.f);

    for (int i=0; i<tl->nofTriangles; ++i) {
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
            //cout << xtemp << " " << ytemp << " " << ztemp << endl;
        }
    } else {
        cout << filename << " could not be opened" << endl;
    }
    infile.close();   
}

void initGL() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(45.f, width/height, 0.1f, 100000.f);
    glMatrixMode(GL_MODELVIEW);
}
    

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.f, 35000.f, 50000.f, // eye
              0.f, 10000.f, 10000.f, // lookat
              0.f, -1.f, 0.f); // up

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    drawTriangles(tl, &x[0], &y[0], &z[0]);

    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}

void reshape(int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, x, y);
    gluPerspective(45.f, x/y, 0.1f, 100000.f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{

    parsePoints("../data/hw4.heights", &x, &y, &z, n);
    cout << "Parsing complete, found " << n << " points" << endl;
    copyCoordinatesToGraph(n, &x[0], &y[0], &z[0], 1, &g);
    planeSweep(g);
    delaunay1(g);
    copyGraphToListOfTriangles(g, &tl);
    cout << "tl->nofTriangles: " << tl->nofTriangles << endl;

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
