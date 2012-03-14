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
vector<Vector3> tourColors;
vector<Vector3> controlPoints;
triangleList *tl;
char *g;
int n, tn;
Camera* camera;
int mouseX, mouseY;

Terrain *terrain;
SightPath * sightPath;
SightPath2 * sightPath2;

float tourT = 0.f;
float tourSpeed = 0.001f;
Vector3 tourPos, nextPos;

float totalLength, minDistance, maxCurvature;

int currentPoi = 2;

bool bDrawTerrain = true;
bool bDrawTours = true;
bool bDrawCurve = true;
bool bDrawControlPoints = true;
bool bDrawControlPolygon = false;
bool bDrawControlPolygonExt = false;
bool bDrawControlPolyFill = false;
bool bDrawToursBB = false;
bool bDrawTourer = true;
bool bFirstPerson = false;
bool wasd[4] = {false,false,false,false};
bool moveTourer [2] = { false, false };

void reshape(int x, int y);
void drawCurves();
void display();

void stepTourer(float _inc) {
	tourT += _inc;
	if (tourT >= 1.f) tourT = 0.0001f;
	else if (tourT < 0.f) tourT = 0.9999f;
	//std::cout << "t=" << tourT << std::endl;
	//std::cout << "nofctrlpts=" << controlPoints.size() << std::endl;
	//for (int i=0; i<controlPoints.size(); ++i) {
	//	std::cout << controlPoints.at(i).x << " " << controlPoints.at(i).y << " " << controlPoints.at(i).z << std::endl;
	//}
}

void updateTourer(const std::vector<Vector3> &_ctrlpts) {
	tourPos = BezierCurve::evaluateGlobal(_ctrlpts, tourT);
	nextPos = BezierCurve::evaluateGlobal(_ctrlpts, tourT+0.001f);
}

void updateMetrics() {
	totalLength = BezierCurve::length(controlPoints, 200);
	minDistance = BezierCurve::minDistance(controlPoints, 100, terrain);
	std::cout << "Total length: " << totalLength << std::endl;
	std::cout << "Min distance: " << minDistance << std::endl;
}

void drawTourer(float _size) {
	glPushMatrix();
	glTranslatef(tourPos.x, tourPos.y, tourPos.z);
	glColor3f(1.f, 0.f, 1.f);
	glutSolidCube(_size);
	glPopMatrix();
}

void drawTriangles()
{
  glColor3f(1.f, 1.f, 1.f);
  terrain->renderTriangles();
}

void drawTour(vector<Vector3> *tour) {
  for (int i=0; i<tour->size(); ++i) {
    glPushMatrix();
    glTranslatef(tour->at(i).x, tour->at(i).y, tour->at(i).z);
    glColor3f(tourColors[i].x,tourColors[i].y,tourColors[i].z);
    glutSolidCube(50);
    glPopMatrix();
  }
}

void drawBoundingTour(vector<Vector3> *tour) {
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i=0; i<tour->size(); ++i) {
    glPushMatrix();
    glTranslatef(tour->at(i).x, tour->at(i).y, tour->at(i).z);
    glColor4f(tourColors[i].x,tourColors[i].y,tourColors[i].z,0.3f);
    glutSolidCube(2*D);
    glPopMatrix();
    }
    glDisable(GL_BLEND);
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
  glOrtho(-20000, 20000.0, -20000, 20000.0, -30.0, 2000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0000.0, 0000.0, 0000.0,
	    0.0, 0.0, 1.0,
	    0.0, 1.0, 0.0);
  //  camera->lookThrough();
  //drawCurves();
  drawTriangles();
  reshape(width,height);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_LIGHTING);
}

void removePoi()
{
  currentPoi = currentPoi % sightPath2->numSights();
  printf("Removing POI(%i)\n",currentPoi);
  sightPath2->removeSight(currentPoi);
  controlPoints = sightPath2->controlPoints();
}

void cyclePoi()
{
  currentPoi = (currentPoi+1) % sightPath2->numSights();  
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
            tourColors.push_back(Vector3(
                    rand() / (float)RAND_MAX,
                    rand() / (float)RAND_MAX,
                    rand() / (float)RAND_MAX));
        }
    } else {
        cout << filename << " could not be opened" << endl;
    }
    infile.close();  
}

void initGL() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glShadeModel(GL_SMOOTH);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(60.f, width/height, 0.1f, 100000.f);
    glMatrixMode(GL_MODELVIEW);
    camera = new Camera(0.0f, 0.0f, 2000.f, 0.f, 0.f);
    camera->posX = 7553.189941;
    camera->posY = -6245.730469;
    camera->posZ = 14367.314453;
    camera->yaw = 142.399872;
    camera->pitch = 36.799988;
}

void drawCurves()
{
  if(bDrawCurve)
    BezierCurve::renderCurves(controlPoints,5000,3.f);
  
  if (bDrawControlPoints)
    BezierCurve::renderCtrlPts(controlPoints, 60);
  
  if (bDrawControlPolygon)
    BezierCurve::renderCtrlPoly(controlPoints,4.f);
  
  if(bDrawControlPolygonExt)
    BezierCurve::renderCtrlPolyExt(controlPoints,4.f);
  
  if (bDrawControlPolyFill)
    BezierCurve::renderCtrlPolyFill(controlPoints);

  if(bDrawToursBB)
    drawBoundingTour(&tour);
    
  if(bDrawTourer) {
	  drawTourer(300.f);
  }
  

}
    
void display() {
  float scale = 10.f;
    if (wasd[0]) camera->walkForward(20.f*scale);
    if (wasd[1]) camera->walkBackwards(20.f*scale);
    if (wasd[2]) camera->strafeLeft(10.f*scale);
    if (wasd[3]) camera->strafeRight(10.f*scale);
    if (moveTourer[0]) stepTourer(tourSpeed);
    if (moveTourer[1]) stepTourer(-tourSpeed);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    
    if (bFirstPerson) {
		 bDrawTourer = false;
		 gluLookAt(tourPos.x, tourPos.y, tourPos.z,
	     nextPos.x, nextPos.y, nextPos.z,
		 0, 0 , 1);
	} else {
		camera->lookThrough();
	}


       
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
    
    updateTourer(controlPoints);
    drawCurves();
    //drawMinimap();

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
         "  |      ___     ___     ____   _ _      ___              |\n"
         "  |     / __|   / __|   |__ /  | | |    ( _ )   __ _      |\n"
         "  |    | (__    \\__ \\    |_ \\  |_  _|   / _ \\  / _` |     |\n"
         "  |     \\___|   |___/   |___/   _|_|_   \\___/  \\__,_|     |\n"
         "  |   _|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"|    |\n"
         "  |    \'-0-0-\'\"\'-0-0-\'\"\'-0-0-\'\"\'-0-0-\'\"\'-0-0-\'\"\'-0-0-\"    |\n"
         "  |         (yes, that is a train. Let's go train!        |\n"
         "  |                                                       |\n"
         "  | Stanford cs348a - Geometric Modeling                  |\n"
         "  |                  Final Project                        |\n"
         "  |       by Per Karlsson  - perk@stanford.edu            |\n"
         "  |          Victor Sand   - vsand@stanford.edu           |\n"
         "  |          Steven Lesser - sklesser@stanford.edu        |\n"
         "  |                                                       |\n"
         "  |    Mouse       - Rotate Camera view                   |\n"
         "  |    Press 'w/s' - Walk forward/backwards               |\n"
         "  |    Press 'a/d' - Strafe left/right                    |\n"
         "  |    Press 'c'   - Cycle Point of Interest              |\n"
         "  |    Press 'r'   - Remove Point of Interest             |\n"
         "  |    Press 'f'   - Toggle first person mode             |\n"
         "  |                                                       |\n"
         "  |    Press '1' - Toggle Terrain                         |\n"
         "  |    Press '2' - Toggle Sights                          |\n"
         "  |    Press '3' - Toggle Sight Path                      |\n"
         "  |    Press '4' - Toggle Control Points                  |\n"
         "  |    Press '5' - Toggle Control Polygon                 |\n"
         "  |    Press '6' - Toggle Control Polygon Extended        |\n"
         "  |    Press '7' - Toggle Control Polygon Filled          |\n"
         "  |    Press '8' - Toggle Sights Bounding Boxsed          |\n"
         "  |    Press '9' - Toggle Tourer Box                      |\n"
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
    case '6':
        bDrawControlPolygonExt = !bDrawControlPolygonExt;
        break;
    case '7':
        bDrawControlPolyFill = !bDrawControlPolyFill;
        break;
    case '8':
        bDrawToursBB = !bDrawToursBB;
        break;
    case '9':
		bDrawTourer = !bDrawTourer;
        break;
    case 'w':
        wasd[0] = true;
        break;
    case 's':
        wasd[1] = true;
        break;
    case 'a':
        wasd[2] = true;
        break;
    case 'd':
        wasd[3] = true;
        break;
    case 'r':
      removePoi();
      break;
    case 'c':
      cyclePoi();
      break;
    case 'q':
		moveTourer[0] = true;
		break;
	case 'e':
		moveTourer[1] = true;
		break;
	case 'f':
		bFirstPerson = !bFirstPerson;
		break;
    }
}

void keyReleased (unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            wasd[0] = false;
            break;
        case 's':
            wasd[1] = false;
            break;
        case 'a':
            wasd[2] = false;
            break;
        case 'd':
            wasd[3] = false;
            break;
        case 'q':
			moveTourer[0] = false;
			break;
		case 'e':
			moveTourer[1] = false;
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

void createSightPaths()
{
  /*// create sight path 1
  printf("sight path loaded\n");
  sightPath->createConstraintTangents();
  printf("constraint trangents made\n");
  sightPath->createControlPoints();
  printf("control points created\n");
  //controlPoints = sightPath->controlPoints();*/

  // create sight path 1
  printf("sight path loaded\n");
  sightPath2->createPath();
  controlPoints = sightPath2->controlPoints();
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
    srand(0);
    parseTour("../data/hw4.tour", &tour);
    cout << "Parsed tour, found " << tour.size()/3 << " sights" << endl;

    terrain = new Terrain("../src/sample.mesh3","../src/sample.triangles3");
    printf("terrain loaded\n");
    terrain->print();

    //tour.resize(9);
    sightPath = new SightPath(terrain, tour);
    sightPath2 = new SightPath2(terrain, tour);    
    createSightPaths();

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
    glutKeyboardUpFunc(keyReleased);
    glutMotionFunc(mouseFunc);
    glutPassiveMotionFunc(mouseMoveFunc);
    printInfo();
    updateMetrics();
    glutMainLoop();
    return 0;
}
