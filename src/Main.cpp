/*
 * Main.cp
 *
 *  Created on: Mar 32, 1920
 *      Author: snopp
 */

#include <triangulation.h>
#include <GL/glut.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int width = 800;
int height = 600;

void readPointsFromFile(string filename) {

    ifstream infile;
    infile.open(filename.c_str());
    if (infile.is_open()) {
        while (!infile.eof()) {
            string line;
            getline(infile, line);
            cout << line << endl;
        }
    } else {
        cout << filename << " could not be opened" << endl;
    }
    infile.close();
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
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("cs348a project");
    initGL();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);

    readPointsFromFile("200.data");


    glutMainLoop();
    return 0;
}
