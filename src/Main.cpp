extern "C" {
    #include <triangulation.h>
}

#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int width = 800;
int height = 600;

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

void copyCoordinatesToGraph(int nofSites,
                            int *x,
                            int *y,
                            int *z,
                            int eliminateDuplicates,
                            char **gExternal) {
    graphType *g;
    *gExternal = (char*)newGraph();
    g = (graphType*)*gExternal;
    

}


void initGL() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(45.f, width/height, 0.1f, 100.f);
    glMatrixMode(GL_MODELVIEW);
}
    

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}

void reshape(int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, x, y);
    gluPerspective(45.f, x/y, 0.1f, 100.f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{

    vector<int> x;
    vector<int> y;
    vector<int> z;
    int n;
    parsePoints("../data/200.data", &x, &y, &z, n);
    cout << "Parsing complete, found " << n << " points" << endl;

    char *g;
    copyCoordinatesToGraph(n, &x[0], &y[0], &z[0], 1, &g);

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
