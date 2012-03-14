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
#include "OptimalPath.h"

using namespace std;

int width = 800;
int height = 600;

int miniWidth = 150;
int miniHeight = 150;

vector<int> x;
vector<int> y;
vector<int> z;
vector<Vector3> tour;
vector<Vector3> optimalTour;
vector<Vector3> tourColors;
vector<Vector3> controlPoints1;
vector<Vector3> controlPoints2;
triangleList *tl;
char *g;
int n, tn;
Camera* camera;
int mouseX, mouseY;

Terrain *terrain;
SightPathInterface * sightPath;
SightPath2 * sightPath1;
SightPath2 * sightPath2;

float tourT = 0.f;
float tourSpeed = 0.0005f;
Vector3 tourPos, nextPos;

float totalLength, minDistance, maxCurvature;
int numberOfCurves;

float lineWidth = 5.f;

enum RenderEnum{
    RENDER_SIGHTPATH1,
    RENDER_SIGHTPATH2,
    RENDER_BOTH,
};

RenderEnum render = RENDER_SIGHTPATH2;

bool bDrawTerrain = true;
bool bDrawTours = true;
bool bDrawCurve = true;
bool bDrawControlPoints = true;
bool bDrawControlPolygon = false;
bool bDrawControlPolygonExt = false;
bool bDrawControlPolyFill = false;
bool bDrawToursBB = true;
bool bDrawTourer = true;
bool bFirstPerson = false;
bool wasd[4] = {false,false,false,false};
bool moveTourer [2] = { false, false };

GLuint texID;

void reshape(int x, int y);
void drawCurves(bool);
void display();

SightPath2* curSightPath()
{
    switch (render){
        case RENDER_SIGHTPATH1:
            return sightPath1;
            break;
        case RENDER_SIGHTPATH2:
            return sightPath2;
            break;
        case RENDER_BOTH:
            return sightPath2;
            break;
    }
}

vector<Vector3> & activeControlPoints()
{
    switch (render){
        case RENDER_SIGHTPATH1:
            return controlPoints1;
            break;
        case RENDER_SIGHTPATH2:
            return controlPoints2;
            break;
        case RENDER_BOTH:
            return controlPoints2;
            break;
    }
}

void updateControlPoints()
{
    controlPoints1 = sightPath1->controlPoints();
    controlPoints2 = sightPath2->controlPoints();
}

void stepTourer(float _inc) {
	tourT += _inc;
	if (tourT >= 1.f) tourT = 0.0001f;
	else if (tourT < 0.f) tourT = 0.9999f;
}

void updateTourer(const std::vector<Vector3> &_ctrlpts) {
	tourPos = BezierCurve::evaluateGlobal(_ctrlpts, tourT);
	nextPos = BezierCurve::evaluateGlobal(_ctrlpts, tourT+0.001f);
}

void updateLength() {
	totalLength = BezierCurve::length(controlPoints2, 100);
	std::cout << "Total length: " << totalLength << std::endl;
}

void updateDistance() {
	minDistance = BezierCurve::minDistance(controlPoints2, 5000, terrain);
	std::cout << "Min distance: " << minDistance << std::endl;
}

void updateCurvature() {
	maxCurvature = BezierCurve::maxCurvature(activeControlPoints(), 1000, 0.001);
	std::cout << "Max curvature: " << maxCurvature << std::endl;
}

void updateNrCurves() {
	numberOfCurves = (controlPoints2.size()-1)/2;
	std::cout << "Nr of curves: " << numberOfCurves << std::endl;
}

void drawTourer(float _size) {
	glPushMatrix();
	glTranslatef(tourPos.x, tourPos.y, tourPos.z);
	glColor3f(1.0f, 1.0f, 0.f);
	glutSolidCube(_size);
	glPopMatrix();
}

void drawTriangles()
{
  glColor3f(1.f, 1.f, 1.f);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texID);
  terrain->renderTriangles();
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}

void addSight()
{
    SightPath2 * sightPath = curSightPath();
    std::cout<<"\n#############################\n"
                 "#                           #\n"
                 "#         ADD SIGHT         #\n"
                 "#                           #\n"
                 "#############################\n\n"
                 "Please enter coords(float float float): ";
    float x,y,z;
    std::cin >> x>>y>>z;
    std::cout << "........\nAdding sight at " << x
            << ", " << y << ", " << z << std::endl;

    std::cout << "Please enter after which #sight it should be added(int): ";
    int idx;
    std::cin >> idx;
    if (idx <0 || idx > activeControlPoints().size()-1) {
        std::cerr << "Bad Index!" << std::endl;
        return;
    }
    //add!
    sightPath->addSight(Vector3(x,y,z), idx);
    activeControlPoints = sightPath->controlPoints();
    wasd[0] = false;
    wasd[1] = false;
    wasd[2] = false;
    wasd[3] = false;
}

void removeSight()
{
    SightPath2 * sightPath = curSightPath();
    std::cout<<"\n#############################\n"
                 "#                           #\n"
                 "#       REMOVE SIGHT        #\n"
                 "#                           #\n"
                 "#############################\n\n"
                 "Please enter idx(int): ";
    int idx;
    std::cin >> idx;
    if (idx <0 || idx > activeControlPoints().size()-1) {
        std::cerr << "Bad Index!" << std::endl;
        return;
    }
    std::cout << "........\nRemoving sight #" << idx << std::endl;
    //remove!
    sightPath->removeSight(idx);
    activeControlPoints = sightPath->controlPoints();
    wasd[0] = false;
    wasd[1] = false;
    wasd[2] = false;
    wasd[3] = false;
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
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, miniWidth, miniHeight);
  glOrtho(-20000.f, 20000.f, -19000.f, 19000.f, 10.0f, 10000.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5000.0,
	    0.0, 0.0, -1.0,
	    0.0, 1.0, 0.0);
  //  camera->lookThrough();
  float saved = lineWidth;
  glDisable(GL_LIGHTING);
  switch (render){
      case RENDER_SIGHTPATH1:
          BezierCurve::renderCurves(controlPoints1,100,1.0f,
                  Vector3(0.15,0.15,0.9));
          break;
      case RENDER_SIGHTPATH2:
          BezierCurve::renderCurves(controlPoints2,100,1.0f,
                  Vector3(0.9,0.15,0.15));
          break;
      case RENDER_BOTH:
          BezierCurve::renderCurves(controlPoints1,100,1.0f,
                  Vector3(0.15,0.15,0.9));
          BezierCurve::renderCurves(controlPoints2,100,1.0f,
                  Vector3(0.9,0.15,0.15));
          break;
  }
  drawTourer(1000.f);
  glEnable(GL_LIGHTING);
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

void loadTexture(const char * texture)
{
    std::cout << "Loading texture: texture.data" << std::endl;
    int c;
    ifstream infile;
    infile.open(texture);
    std::vector<unsigned char> pixelData;
    if (infile.is_open()) {
        while (!infile.eof()) {
            infile >> c;
            if (infile.eof()) break;
            //std::cerr << c << std::endl;
            //std::cerr << static_cast<unsigned char>(c) << std::endl;

            pixelData.push_back(static_cast<unsigned char>(c));
        }
    } else {
        cout << "texture could not be opened" << endl;
    }

    infile.close();

    int res = sqrt(pixelData.size()/3);

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, res, res, 0, GL_RGB,
            GL_UNSIGNED_BYTE,&pixelData[0]);
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            //GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "Done loading texture." << std::endl;

}

void initGL(const char * texture) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glShadeModel(GL_SMOOTH);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(60.f, width/height, 0.1f, 100000.f);
    //glOrtho(-20000, 20000.0, -20000, 20000.0, -30.0, 2000);

    glMatrixMode(GL_MODELVIEW);
    loadTexture(texture);
    camera = new Camera(0.0f, 0.0f, 2000.f, 0.f, 0.f);
    camera->posX = 7553.189941;
    camera->posY = -6245.730469;
    camera->posZ = 14367.314453;
    camera->yaw = 142.399872;
    camera->pitch = 36.799988;
}

void drawCurves(bool renderFirst)
{
    std::vector<Vector3> * controlPoints = renderFirst ? &controlPoints1: &controlPoints2 ;
    Vector3 color = renderFirst ? Vector3(0.15,0.15,0.9) : Vector3(0.9,0.15,0.15);

  if(bDrawCurve)
    BezierCurve::renderCurves(*controlPoints,100,lineWidth,color);
  
  if (bDrawControlPoints)
    BezierCurve::renderCtrlPts(*controlPoints, 60);
  
  if (bDrawControlPolygon)
    BezierCurve::renderCtrlPoly(*controlPoints,4.f);
  
  if(bDrawControlPolygonExt)
    BezierCurve::renderCtrlPolyExt(*controlPoints,4.f);
  
  if (bDrawControlPolyFill)
  BezierCurve::renderCtrlPolyFill(*controlPoints);

}
    
void display() {
  float scale = 2.f;
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

    /*glDisable(GL_LIGHTING);
    glLineWidth(15);
    glColor3f(1.f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(-23469, -19540.000000, -2 );
    glVertex3f(23469,  -19540.000000, -2);

    glVertex3f(23469, -19540.000000, -2 );
    glVertex3f(23469,  19540.000000, -2);

    glVertex3f(-23469, -19540.000000, -2 );
    glVertex3f(-23469,  19540.000000, -2);

    glVertex3f(-23469, 19540.000000, -2 );
    glVertex3f(23469,  19540.000000, -2);

    glEnd();
       
    glEnable(GL_LIGHTING);*/
    GLfloat lightPos[] = { 0.0, -1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    if(bDrawTerrain)
        drawTriangles();
    glDisable(GL_LIGHTING);
    if (bDrawTours)
        drawTour(&tour);
    
    updateTourer(activeControlPoints());


    if(bDrawToursBB)
      drawBoundingTour(&tour);

    if(bDrawTourer) {
        drawTourer(300.f);
    }

    switch (render){
        case  RENDER_SIGHTPATH1:
            drawCurves(true);
            break;
        case RENDER_SIGHTPATH2:
            drawCurves(false);
            break;
        case RENDER_BOTH:
            drawCurves(true);
            drawCurves(false);
            break;
    }
    drawMinimap();

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
         "  |    Press 'q/e' - Move tour object                     |\n"
         "  |                                                       |\n"
         "  |    Press '1' - Display original path                  |\n"
         "  |    Press '2' - Display optimal path                   |\n"
         "  |    Press '3' - Display both paths                     |\n"
         "  |                                                       |\n"
         "  |    Press '4' - Toggle Path                            |\n"
         "  |    Press '5' - Toggle Control Points                  |\n"
         "  |    Press '6' - Toggle Control Polygon                 |\n"
         "  |    Press '7' - Toggle Control Polygon Extended        |\n"
         "  |    Press '8' - Toggle Control Polygon Filled          |\n"
         "  |    Press '9' - Toggle Sights Bounding Box             |\n"
         "  |    Press 'x' - Toggle Tourer Box                      |\n"
         "  |    Press 't' - Toggle Terrain                         |\n"
         "  |    Press 'z' - Toggle Sights                          |\n"
         "  |                                                       |\n"
         "  |    Press 'c'   - Add New Sight                        |\n"
         "  |    Press 'r'   - Remove Sight                         |\n"
         "  |    Press 'f'   - Toggle first person mode             |\n"
		 "  |    Press 'v'   - Calculate number of arcs             |\n"
		 "  |    Press 'b'   - Calculate total length               |\n"
		 "  |    Press 'n'   - Calculate min distance               |\n"
		 "  |    Press 'm'   - Calculate max curvature              |\n"
		 "  |                                                       |\n"
         "  |    Press 'esc' - Quit                                 |\n"
         "  ---------------------------------------------------------"
        << std::endl;

}

void keyPressed (unsigned char key, int x, int y) {  
    switch (key) {
    case 't':
        bDrawTerrain = !bDrawTerrain;
        break;
    case 'z':
        bDrawTours = !bDrawTours;
        break;
    case '4':
        bDrawCurve = !bDrawCurve;
        break;
    case '5':
        bDrawControlPoints = !bDrawControlPoints;
        break;
    case '6':
        bDrawControlPolygon = !bDrawControlPolygon;
        break;
    case '7':
        bDrawControlPolygonExt = !bDrawControlPolygonExt;
        break;
    case '8':
        bDrawControlPolyFill = !bDrawControlPolyFill;
        break;
    case '9':
        bDrawToursBB = !bDrawToursBB;
        break;
    case 'x':
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
      removeSight();
      break;
    case 'c':
      addSight();
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
	case '2':
	    std::cout << "Displaying optimal path" << std::endl;
	    render = RENDER_SIGHTPATH1;
	    break;
	case '1':
	    std::cout << "Displaying original path" << std::endl;
	    render = RENDER_SIGHTPATH2;
	    break;
	case '3':
	    render = RENDER_BOTH;
	    break;
	case 'b':
		updateLength();
		break;
	case 'n':
		updateDistance();
		break;
	case 'm':
		updateCurvature();
		break;
	case 'v':
		updateNrCurves();
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
    sightPath1 = new SightPath2(terrain, optimalTour);
    sightPath1->createPath();

    sightPath2 = new SightPath2(terrain, tour);
    sightPath2->createPath();

    updateControlPoints();
    //sightPath2->print();

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
    const char * texturename =  argc == 2 ? argv[1] : "../src/texture.data";
    std::cerr << texturename << std::endl;
    srand(0);
    parseTour("../data/hw4.tour", &tour);
    cout << "Parsed tour, found " << tour.size() << " sights" << endl;
    optimalTour = optimalSightPath(tour);

    terrain = new Terrain("../src/sample.mesh3","../src/sample.triangles3");
    //printf("terrain loaded\n");
    terrain->print();

    createSightPaths();

    //printf("everything made\n");
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("cs348a project");
    initGL(texturename);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutMotionFunc(mouseFunc);
    glutPassiveMotionFunc(mouseMoveFunc);
    printInfo();
    glutMainLoop();
    return 0;
}
