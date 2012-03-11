/*
 * Main.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: per
 */

#include <triangulation.h>
#include <GL/glut.h>

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("cs348a project");

    /*glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(processNormalKeys);
    glutMotionFunc(mouseFunc);*/
    glutMainLoop();
    return 0;
}
