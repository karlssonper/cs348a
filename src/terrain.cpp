#include "terrain.h"
#include <stdio.h>
#include <string.h>

Point::Point(int _x, int _y, int _z)
{
  x = _x;
  y = _y;
  z = _z;
}

//////////////////////////// TERRAIN ////////////////////////////////T
Terrain::Terrain(char* fileVertices, char* fileTriangles)
{
  boundsFound = false;
  ReadTerrain (fileVertices);
  ReadTriangles(fileTriangles);
  CreateNormals();
  float scratch;
  getBounds(&scratch,&scratch,
	    &scratch,&scratch,&scratch,&scratch);
  constructGrid(16,16);
  CreateColors();
}

Point Terrain::getGrid(Vector3 val) const
{
  float xStride = (maxBound.x-minBound.x) / numGridCols;
  float yStride = (maxBound.y-minBound.y) / numGridRows;
  int x=numGridCols-1;
  int y=numGridRows-1;
  for (int i = 1; i <= numGridCols; i++)
    {
      if (val.x <= minBound.x + i*xStride)
	{
	  x = i-1;
	  break;
	}
    }

  for (int i = 1; i <= numGridRows; i++)
    {
      if (val.y <= minBound.y + i*yStride)
	{
	  y = i-1;
	  break;
	}
    }
  return Point(x,y,0);
}

inline int min(int a, int b)
{
  return a < b ? a : b;
}

inline int max(int a, int b)
{
  return a > b ? a : b;
}

std::vector<Triangle> Terrain::getTriangles(Vector3 p1, Vector3 p2) const
{
  std::vector<Triangle> tri;
  // determine candidate grid cells
  Point grid1 = getGrid(p1);
  Point grid2 = getGrid(p2);
  std::vector<Point> gridCells;  
  int minX = max(0, min(grid1.x-1, grid2.x-1));
  int minY = max(0, min(grid1.y-1, grid2.y-1));
  int maxX = min(numGridCols, max(grid1.x+1, grid2.x+1));
  int maxY = min(numGridRows, max(grid1.y+1, grid2.y+1));
  printf("min(%i,%i) max(%i,%i)\n",minX,minY,maxX,maxY);
  for (int col = minX; col <= maxX; col++)
    {
      for (int row = minY; row <= maxY; row++)
	{
	  Point gridCell = Point(col,row,0);
	  gridCells.push_back(gridCell);
	} 
    }
  if (gridCells.size() > 9)
    printf(" lots of grid cells\n");
  // add all the elements in the grid cells to the result vector
  for (int i = 0; i < gridCells.size(); i++)
    {
      Point gridCell = gridCells[i];
      int idx = gridCell.y*numGridCols+gridCell.x;
      for (int j = 0; j < grid[idx].size(); j++)
	{
	  Point triangleIndices = triangles[grid[idx][j]];
	  Vector3 p1 = points[triangleIndices.x];
	  Vector3 p2 = points[triangleIndices.y];
	  Vector3 p3 = points[triangleIndices.z];
	  Triangle t(p1,p2,p3);
	  tri.push_back(t);
	}
    }
  return tri;
}

void Terrain::constructGrid(int rows, int cols)
{
  numGridRows = rows;
  numGridCols = cols;
  // initialize grid cells
  for (int i = 0; i < rows*cols; i++)
    {
      grid.push_back(std::vector<int>());
    }

  // determine grid location for each point
  float xStride = (maxBound.x - minBound.x) / float(cols);
  float yStride = (maxBound.y - minBound.y) / float(rows);
  Point *indices = new Point[points.size()];
  for (unsigned int i = 0; i < points.size(); i++)
    {
      Vector3 p = points[i];
      Point gridBlock = getGrid(p);
      if (gridBlock.x < 0 || gridBlock.y < 0) printf("bad grid value\n");
      indices[i] = gridBlock;
    }
  // assign each triangle a grid cell
  for (unsigned int i = 0; i < triangles.size(); i++)
    {
      Point tri = triangles[i];
      Point gridValue = indices[tri.x];
      grid[gridValue.y*cols+gridValue.x].push_back(i);
    }
  free (indices);
}

void Terrain::renderTriangles()
{
  /*glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &(points[0].x));
  glNormalPointer(GL_FLOAT, 0, &(normals[0].x));
  glDrawElements(GL_TRIANGLES, triangles.size()*3, GL_UNSIGNED_INT,
		 &(triangles[0].x));
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);*/
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < triangles.size(); i++)
    {
      Point tri = triangles[i];
      Vector3 x = points[tri.x];
      Vector3 y = points[tri.y];
      Vector3 z = points[tri.z];
      Vector3 n = normals[i];
      glNormal3f(n.x, n.y, n.z);
      glVertex3f(x.x, x.y, x.z);
      glVertex3f(y.x, y.y, y.z);
      glVertex3f(z.x, z.y, z.z);
    }
    glEnd();
}

void Terrain::print()
{
  printf("Terrain contains %d points and %d triangles\n",points.size(),
	 triangles.size());
}

void Terrain::getBounds(float *minx, float *maxx, float *miny, float *maxy, 
			float *minz, float *maxz)
{
  if (!boundsFound)
    {
      float minX = points[0].x;
      float maxX = points[0].x;
      float minY = points[0].y;
      float maxY = points[0].y;
      float minZ = points[0].z;
      float maxZ = points[0].z;
      float x,y,z;
      for (unsigned int i = 0; i < points.size(); i++)
	{
	  x = points[i].x;
	  y = points[i].y;
	  z = points[i].z;
	  if (x < minX)
	    minX = x;
	  else if (x > maxX)
	    maxX = x;
	  if (y < minY)
	    minY = y;
	  else if (y > maxY)
	    maxY = y;
	  if (z < minZ)
	    minZ = z;
	  else if (z > maxZ)
	    maxZ = z;      
	}
      minBound = Vector3(minX,minY,minZ);
      maxBound = Vector3(maxX,maxY,maxZ);
      printf("Terrain Bounds \n  x(%f, %f) \n  y(%f, %f) \n  z(%f, %f)\n", minX, maxX,
	     minY, maxY, minZ, maxZ);
    }
      
  *minx = minBound.x;
  *miny = minBound.y;
  *minz = minBound.z;
  *maxx = maxBound.x;
  *maxy = maxBound.y;
  *maxz = maxBound.z;
  boundsFound = true;
}

void Terrain::ReadTerrain (char* fileName)
{
  char buffer[1024];
  FILE *fp;
  int ret = 5;
  fp = fopen (fileName, "r");
  int x, y, z, index;
  // skip first line
  while (ret == 5)
    {
      ret = fscanf (fp, "%s %d %d %d %d\n", buffer, &x, &y, &z, &index);
      if (strcmp(buffer, "site") != 0)
	break;
      points.push_back(Vector3(x,y,z));
    }
}

void Terrain::ReadTriangles(char* fileName)
{
  char buffer[1024];
  FILE *fp;
  int ret = 3;
  fp = fopen (fileName, "r");
  int x,y,z,index;
  // skip first line
  while (true)
    {
      ret = fscanf (fp, "%d %d %d\n", &x, &y, &z);
      if (ret != 3)
	break;
      triangles.push_back(Point(x,y,z));
    }
}

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

void Terrain::CreateNormals()
{
  float nx, ny, nz;
  for (int i = 0; i < triangles.size(); i++)
    {
      Point t = triangles[i];
      Vector3 x = points[t.x];
      Vector3 y = points[t.y];
      Vector3 z = points[t.z];
      calculateNormal(x.x, x.y, x.z,
		      y.x, y.y, y.z,
		      z.x, z.y, z.z,
		      &nx, &ny, &nz);
      normals.push_back(Vector3(nx,ny,nz));
    }
}

// taken from http://www.lighthouse3d.com/tutorials/glsl-tutorial/setup-for-glsl-example/
void Terrain::CreateColors()
{
  float minZ = minBound.z;
  float maxZ = maxBound.z;
  float range = maxZ - minZ;
  float scale;
  for (unsigned int i = 0; i < points.size(); i++)
    {
      Vector3 v = points[i];
      scale = (v.z-minZ) / range;
      heights.push_back(scale);
    }
}
