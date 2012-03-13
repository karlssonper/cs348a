/*
 * SightPath.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: per
 */

#include "SightPath.h"
#include "Ray.h"
#include "IntersectionInfo.h"
#include "terrain.h"
#include <limits>
#include <stdio.h>

SightPath::SightPath(const Terrain * terrain,
          const std::vector<Vector3> &sights) : terrain_(terrain)
{
    sights_.resize(sights.size());
    for (int i = 0; i < sights.size(); ++i) {
        sights_[i].pos = sights[i];
        sights_[i].pos.z += D;
    }
}

Vector3 SightPath::tangent(Vector3 p0, Vector3 p1)
{
    Vector3 dir = p1 - p0;
#define PI 3.14159265
    float phi = atan2(dir.y, dir.x);
    float theta = 135 *PI/180.0;
    Vector3 t(cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta));
    return t;
}

void SightPath::createConstraintTangents()
{
    //first point's tangent is the direction to the first sight
    sights_.front().tangent = tangent(sights_[0].pos,sights_[1].pos);
    for (int i = 1; i < sights_.size() - 1; ++i) {
        sights_[i].tangent = tangent(sights_[i-1].pos,sights_[i+1].pos);
    }

    //last point's tangent is direction of the second last point.
    sights_.back().tangent = tangent(sights_[sights_.size()-2].pos,
                                     sights_[sights_.size()-1].pos);
}

void SightPath::createControlPoints()
{
    controlPoints_.clear();
    for (int i = 0; i < sights_.size() - 1; ++i){
        solve(sights_[i],sights_[i + 1]);
    }
    controlPoints_.push_back(sights_.back().pos);
}

void SightPath::removeSight(int i)
{
    sights_.erase(sights_.begin()+i);
}

std::vector<Vector3> SightPath::sights() const
{
    std::vector<Vector3> pos(sights_.size());
    for (int i = 0; i < sights_.size(); ++i){
        pos[i] = sights_[i].pos;
    }
    return pos;
}

void SightPath::solve (Sight sight0, Sight sight1)
{
    Vector3 cp0 = sight0.pos, cp1 = sight1.pos, mp;
    bool ok = true;
    do {
        cp0 = cp0 + sight0.tangent *  CP_SCALE;
        cp1 = cp1 + sight1.tangent * -CP_SCALE;
        mp = (cp0+cp1)/2.f;
        ok = intersection(sight0.pos,mp) ||
             intersection(cp0,mp) ||
             intersection(mp,sight1.pos) ||
             intersection(mp,cp1);
    } while (ok);

    controlPoints_.push_back(sight0.pos);
    controlPoints_.push_back(cp0);
    controlPoints_.push_back(mp);
    controlPoints_.push_back(cp1);
}

Vector3 SightPath::lineIntersect(Vector3 v0,
                                          Vector3 v1,
                                          Vector3 v2,
                                          Vector3 v3) const
{
    float den = v3.x*v1.y - v3.y*v1.x;
    if (den == 0.0f) std::cerr << "AAAH LINEINTERSECT ERROR!!";
    float t2 = (v1.x*(v2.y-v0.y) - v1.y * (v2.x-v0.x)/den);
    float t1 = (v2.x-v0.x + t2*v3.x)/v1.x;

    Vector3 vt1 = v0 + v1*t1;
    Vector3 vt2 = v2 + v3*t2;

    printf("VT1 %f %f %f\n", vt1.x, vt1.y, vt1.z);
    printf("VT2 %f %f %f\n", vt2.x, vt2.y, vt2.z);

    return v2 + v3*t2;
}

bool SightPath::intersection(const Vector3 & source, const Vector3 &dest)
{
    std::vector<Triangle> triangles = terrain_->getTriangles(source,dest);
    Vector3 dir = (dest-source);
    Ray r(source,dir,0, 1.0f);
    for (int i = 0; i < triangles.size(); ++i) {
        IntersectionInfo ii = triangles[i].rayIntersect(r);
        //if (ii.t < dir.mag()) {
        if (ii.hit) {
            std::cout << "t" <<  ii.t << std::endl;
            return true;

        }
    }
    return false;
}

///////////////////////////// SIGHT PATH 2 ////////////////////////////

SightPath2::SightPath2(const Terrain * terrain,
		       const std::vector<Vector3> &sights) : terrain_(terrain), isPathValid_(0)
{
  
  sights_.resize(sights.size());
  for (int i = 0; i < sights.size(); ++i)
    sights_[i] = sights[i] + Vector3(0,0,D);
  siteSegments_.resize(sights.size());
}

const std::vector<Vector3> SightPath2::controlPoints()
{
  if (!isPathValid_)
    {
      path_.clear();
      for (unsigned int i = 0; i < numSights(); i++)
	{
	  SiteSegment s = siteSegments_[i];
	  if (s.t == SiteType_First)
	    {
	      path_.push_back(s.p1);
	      path_.push_back(s.ctrl2);
	    }
	  else if (s.t == SiteType_Middle)
	    {
	      path_.push_back(s.p0);
	      path_.push_back(s.ctrl1);
	      path_.push_back(s.p1);
	      path_.push_back(s.ctrl2);
	    }
	  else if (s.t == SiteType_Last)
	    {
	      path_.push_back(s.p0);
	    }
	}
      isPathValid_ = 1;
    }
  return path_;
}

Vector3 SightPath2::getMidPoint(int index1, int index2)
{
  if (index1 < 0 || index1 > numSights() ||
      index2 < 0 || index2 > numSights())
    {
      printf("bad mid point\n");
      return Vector3(0,0,0);
    }
  Vector3 p1 = sights_[index1];
  Vector3 p2 = sights_[index2];
  return p1 + (p2-p1)*0.5f;
}

int SightPath2::getLength(int index1, int index2)
{
  if (index1 < 0 || index1 > numSights() ||
      index2 < 0 || index2 > numSights())
    return -1;
  Vector3 diff = sights_[index2] - sights_[index1];
  return diff.mag();
}

int SightPath2::solveFirstSegment()
{
  SiteSegment s;
  s.t = SiteType_First;
  s.p1 = sights_[0];
  s.ctrl2 = getMidPoint(0,1);
  siteSegments_[0] = s;
  return 1;
}

int SightPath2::solveLastSegment()
{
  SiteSegment s;
  s.t = SiteType_Last;
  s.p0 = sights_[numSights()-1];
  s.ctrl0 = siteSegments_[numSights()-2].ctrl2;
  siteSegments_[numSights()-1] = s;
  return 1;
}

void SightPath2::createPath()
{
  siteSegments_.clear();
  siteSegments_.resize(numSights());
  for (unsigned int i = 0; i < numSights(); i++)
    solveSiteSegment(i);
}
 
void SightPath2::removeSight(int index)
{
  for (unsigned int i = index; i < numSights()-1; i++)
    {
      siteSegments_[i] = siteSegments_[i+1];
    }
  siteSegments_.pop_back();
  if (index == numSights()-1)
    solveSiteSegment(index);
  else
    {
      solveSiteSegment(index);
      solveSiteSegment(index+1);
    }
  isPathValid_ = 0;
}

int SightPath2::solveMidSegment(int index)
{
  SiteSegment s;
  s.t = SiteType_Middle;

  Vector3 cp0 = siteSegments_[index-1].ctrl2; // previous mid control point
  Vector3 cp2 = getMidPoint(index, index+1); // next mid control point
  // raise midpoint up by a factor of the length between the two indices
  Vector3 upDirection(0,0,1);
  float midPointPushAmount = getLength(index,index+1)*MID_RAISE_FACTOR;
  cp2 = cp2 + upDirection*midPointPushAmount;
  Vector3 poi = sights_[index]; // current poi to visit
  Vector3 v1 = (poi - cp0).normalize();
  Vector3 v2 = (poi - cp2).normalize();
  Vector3 v = (v1+v2).normalize();
  float scaleFactor = START_CURVE_SCALE;
  // while loop
  Vector3 cp1 = poi + v*scaleFactor; // control point for current site
  Vector3 v3 = (cp0-cp1).normalize(); // vector from cp1 back to cp0
  Vector3 v4 = (cp2-cp1).normalize(); // vector from cp1 forward to cp2
  float theta = acos(v3.dot(v4)); // angle between v3 and v4
  float L = scaleFactor / (cos(theta / 2.f)); // scale of v3 and v4 relative to v
  Vector3 p0 = cp1 + v3*(2.f*L); // point between cp0 and cp1
  Vector3 p1 = cp1 + v4*(2.f*L); // point between cp1 and cp2

  // store values
  s.ctrl0 = cp0;
  s.ctrl1 = cp1;
  s.ctrl2 = cp2;
  s.p0 = p0;
  s.p1 = p1;
  siteSegments_[index] = s;
  return 1;
}

// 1 on success, 0 on failure
int SightPath2::solveSiteSegment(int index)
{
  SiteSegment s;
  // specially handle first and last segment
  if (index == 0)
      return solveFirstSegment();
  else if (index == numSights()-1)
      return solveLastSegment();
  // handle middle segment
  else if (index > 0 && index < numSights()-1)
      return solveMidSegment(index);
  // invalid index
  else
    return -1;
  
}

