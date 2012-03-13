#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <GL/glut.h>
#include <vector>
#include "MathEngine.h"
#include "Triangle.h"

class Point
{
 public:
  Point(int _x=0, int _y=0, int _z=0);
  int x;
  int y;
  int z;
};

class Terrain
{
 public:
  Terrain(char* fileVertices, char* fileTriangles);

  void renderTriangles();
  void print();
  void getBounds(float *minx, float *maxx, 
		 float *miny, float *maxy, 
		 float *minz, float *maxz);
  std::vector<Triangle> getTriangles(Vector3 p1, Vector3 p2) const;

  std::vector<Vector3> points;
  std::vector<Point> triangles;
  std::vector<Vector3> normals;
  std::vector<float> heights;
  std::vector<std::vector<int> > grid;

 private:
  void ReadTerrain (char* fileName);
  void ReadTriangles(char* fileName);
  void constructGrid(int rows, int cols);
  Point getGrid(Vector3 val) const;
  void CreateNormals();
  void CreateColors();

  bool boundsFound;
  Vector3 minBound;
  Vector3 maxBound;
  int numGridCols;
  int numGridRows;
};

#endif
