/*
 * SightPath.h
 *
 *  Created on: Mar 12, 2012
 *      Author: per
 */

#ifndef SIGHTPATH_H_
#define SIGHTPATH_H_

#include <vector>
#include "MathEngine.h"

#define CP_SCALE 500.0f
#define D 200.f

#define MID_RAISE_FACTOR 0.08f
#define START_CURVE_SCALE 2000.f
#define NUM_BEZIER_COLLISION_TESTS 10
class Terrain;

bool intersection(const Vector3 & source, const Vector3 &dest, const Terrain *terrain_);
bool intersection(const Vector3 & v0, const Vector3 &v1, const Vector3 &v2, const Terrain *terrain_);
// intersection for bezier curve made of points p0,p1,p2
bool intersectionBezier(const Vector3 & p0, const Vector3 & p1, const Vector3 &p2, const Terrain *terrain_);

class SightPathInterface
{
 public:
  SightPathInterface(){};
  virtual ~SightPathInterface(){};
  virtual const std::vector<Vector3> controlPoints()=0;
  virtual void removeSight(int index)=0;
  virtual void addSight(Vector3 pos, int prevSightIdx) = 0;
};
  

class SightPath1
: public SightPathInterface
{
public:
    SightPath1(const Terrain * terrain,
              const std::vector<Vector3> &sights);

    void createConstraintTangents();
    void createControlPoints();
    void removeSight(int index);
    const std::vector<Vector3> controlPoints() {return controlPoints_;};
    std::vector<Vector3> sights() const;
    void addSight(Vector3 pos, int prevSightIdx);
private:
    struct Sight {
        Vector3 pos;
        Vector3 tangent;
    };
    std::vector<Sight> sights_;
    std::vector<Vector3> controlPoints_;
    const Terrain * terrain_;

    void solve (Sight sight0, Sight sight1);
    Vector3 tangent(Vector3 p0, Vector3 p1);
};


// Alternate method
class SightPath2
: public SightPathInterface
{
public:
    SightPath2(const Terrain * terrain,
              const std::vector<Vector3> &sights);

    void createPath();
    void removeSight(int index);
    int numSights() {return sights_.size();}
    int moveMidpoint(int index, Vector3 diff);
    void addSight(Vector3 pos, int prevSightIdx);
    const std::vector<Vector3> controlPoints();
private:
    void createMidpoints();
    int solveSiteSegment(int index, bool optimize=false);    
    int solveFirstSegment(bool optimize=false);
    int solveLastSegment(bool optimize=false);
    int solveMidSegment(int index, bool optimize=false);
    Vector3 getMidPoint(int index1, int index2);
    int getLength(int index1, int index2);
    enum SiteType
    {
      SiteType_First,
      SiteType_Middle,
      SiteType_Last
    };
    struct SiteSegment
    {
      SiteType t;
      Vector3 p0;
      Vector3 ctrl;
      Vector3 p1;
    };
    std::vector<SiteSegment> siteSegments_;
    std::vector<Vector3> midpoints_;
    std::vector<Vector3> sights_;
    std::vector<Vector3> path_;
    int isPathValid_;
    const Terrain * terrain_;

};

#endif /* SIGHTPATH_H_ */
