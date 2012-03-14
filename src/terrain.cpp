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

  float scratch;
  getBounds(&scratch,&scratch,
	    &scratch,&scratch,&scratch,&scratch);
  constructGrid(16,16);
  CreateNormals();
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
  int maxX = min(numGridCols-1, max(grid1.x+1, grid2.x+1));
  int maxY = min(numGridRows-1, max(grid1.y+1, grid2.y+1));
  //printf("min(%i,%i) max(%i,%i)\n",minX,minY,maxX,maxY);
  int numElements = 0;
  for (int col = minX; col <= maxX; col++)
    {
      for (int row = minY; row <= maxY; row++)
	{
	  Point gridCell = Point(col,row,0);
	  gridCells.push_back(gridCell);
	  numElements += grid[gridCell.y*numGridCols+gridCell.x].size();
	} 
    }
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
  //glDisable(GL_LIGHTING);
  //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  //glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < triangles.size(); i++)
    {
      Point tri = triangles[i];
      Vector3 x = points[tri.x]; Vector3 cx = colors[tri.x];
      Vector3 y = points[tri.y]; Vector3 cy = colors[tri.x];
      Vector3 z = points[tri.z]; Vector3 cz = colors[tri.x];
      Vector3 n = normals[i];

      /*Vector3 nx = vertexNormals[tri.x];
      Vector3 ny = vertexNormals[tri.y];
      Vector3 nz = vertexNormals[tri.z];*/

      float ux = uv[tri.x].x;
      float uy = uv[tri.y].x;
      float uz = uv[tri.z].x;

      float vx = uv[tri.x].y;
      float vy = uv[tri.y].y;
      float vz = uv[tri.z].y;

      //glNormal3f(n.x, n.y, n.z);
      //glColor3f(cx.x, cx.y, cx.z);
      glTexCoord2f(ux,vx);
      //glNormal3f(nx.x, nx.y, nx.z);
      glVertex3f(x.x, x.y, x.z);
      //glColor3f(cy.x, cy.y, cy.z);
      glTexCoord2f(uy,vy);
      //glNormal3f(ny.x, ny.y, ny.z);
      glVertex3f(y.x, y.y, y.z);
      //glColor3f(cz.x, cz.y, cz.z);
      glTexCoord2f(uz,vz);
      //glNormal3f(nz.x, nz.y, nz.z);
      glVertex3f(z.x, z.y, z.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
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
      float u = (x+23469)/(2.0*23469);
      float v = (y+19540)/(2.0*19540);
      uv.push_back(Vector3(u,v,1));
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

  /*vertexNormals.resize(points.size());
  for (int i = 0; i < points.size(); ++i) {
      //std::cout << "VERTEX NORMAL #" << i << std::endl;
      Vector3 p = points[i];
      std::vector<Triangle> tris = getTriangles(p,p);
      std::cout << tris.size() << std::endl;
      std::vector<Vector3> neighborTrisNormals;
      for (int j = 0; j < tris.size(); ++j) {
          //std::cout << "POSSIBLE NAERBY TRI #" << j << std::endl;
          if ((tris[j].v1.x == p.x &&
               tris[j].v1.y == p.y &&
               tris[j].v1.z == p.z)
               ||
               (tris[j].v2.x == p.x &&
               tris[j].v2.y == p.y &&
               tris[j].v2.z == p.z)
               ||
               (tris[j].v3.x == p.x &&
               tris[j].v3.y == p.y &&
               tris[j].v3.z == p.z)) {
              neighborTrisNormals.push_back(tris[j].n);
          }
      }

      Vector3 n;
      for (int j = 0; j < neighborTrisNormals.size(); ++j) {
          //std::cout << " NAERBY TRI #" << j << std::endl;
          n = n + neighborTrisNormals[j];
      }
      vertexNormals[i] = n/float(neighborTrisNormals.size());
  }*/
}

Vector3 blend(Vector3 c1, Vector3 c2, float f1, float f2, float amount)
{
  float blendFac = (amount-f1) / (f2-f1);
  Vector3 diff = c2-c1;
  return c1 + diff*blendFac;
}

// taken from http://www.lighthouse3d.com/tutorials/glsl-tutorial/setup-for-glsl-example/
void Terrain::CreateColors()
{
  float minZ = minBound.z;
  float maxZ = maxBound.z;
  float range = maxZ - minZ;
  float scale;
  Vector3 color;
  Vector3 colorTable[] = {Vector3(0,0,255)/255.f,
			Vector3(0,128,255)/255.f,
			Vector3(240,240,64)/255.f,
			Vector3(32,160,0)/255.f,
			Vector3(224,224,0)/255.f,
			Vector3(128,128,128)/255.f,
			Vector3(255,255,255)/255.f};
  float colorCuts[] = {-0.25f,
		       0.01f,
		       0.0625f,
		       0.125f,
		       0.375f,
		       0.75f,
		       2.f};
  int numColors = 7;
  
  colors.clear();
  for (unsigned int i = 0; i < points.size(); i++)
    {
      Vector3 v = points[i];
      scale = (v.z-minZ) / range;
      for (unsigned int j = 0; j < numColors; j++)
	{
	  if (scale <= colorCuts[j])
	    {
	      color = blend(colorTable[j-1], colorTable[j],colorCuts[j-1],colorCuts[j],scale);
	      break;
	    }
	}
      colors.push_back(color);
    }
}
