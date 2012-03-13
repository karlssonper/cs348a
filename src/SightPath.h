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

class Terrain;
class SightPath
{
public:
    SightPath(const Terrain * terrain,
              const std::vector<Vector3> &sights);

    void createConstraintTangents();
    void createControlPoints();
    void removeSight(int i);
    const std::vector<Vector3> & controlPoints() const {return controlPoints_;};
    std::vector<Vector3> sights() const;
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
    bool intersection(const Vector3 & source, const Vector3 &dest);
    bool intersection(const Vector3 & v0, const Vector3 &v1, const Vector3 &v2);
};


// Alternate method
class SightPath2
{
public:
    SightPath2(const Terrain * terrain,
              const std::vector<Vector3> &sights);

    void createPath();
    void removeSight(int index);
    const std::vector<Vector3> controlPoints();
    int numSights() {return sights_.size();}
private:
    int solveSiteSegment(int index);    
    int solveFirstSegment();
    int solveLastSegment();
    int solveMidSegment(int index);
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
      Vector3 ctrl0;
      Vector3 p0;
      Vector3 ctrl1;
      Vector3 p1;
      Vector3 ctrl2;      
    };
    std::vector<SiteSegment> siteSegments_;
    std::vector<Vector3> sights_;
    std::vector<Vector3> path_;
    int isPathValid_;
    const Terrain * terrain_;

};

#endif /* SIGHTPATH_H_ */
