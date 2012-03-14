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
#include "BezierCurve.h"

SightPath1::SightPath1(const Terrain * terrain,
          const std::vector<Vector3> &sights) : terrain_(terrain)
{
    sights_.resize(sights.size());
    for (int i = 0; i < sights.size(); ++i) {
        sights_[i].pos = sights[i];
        sights_[i].pos.z += D;
    }
}

Vector3 SightPath1::tangent(Vector3 p0, Vector3 p1)
{
    Vector3 dir = p1 - p0;
#define PI 3.14159265
    float phi = atan2(dir.y, dir.x);
    float theta = 135 *PI/180.0;
    Vector3 t(cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta));
    return t;
}

void SightPath1::createConstraintTangents()
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

void SightPath1::createControlPoints()
{
    controlPoints_.clear();
    for (int i = 0; i < sights_.size() - 1; ++i){
        solve(sights_[i],sights_[i + 1]);
    }
    controlPoints_.push_back(sights_.back().pos);
}

void SightPath1::removeSight(int index)
{
    sights_.erase(sights_.begin()+index);
}

std::vector<Vector3> SightPath1::sights() const
{
    std::vector<Vector3> pos(sights_.size());
    for (int i = 0; i < sights_.size(); ++i){
        pos[i] = sights_[i].pos;
    }
    return pos;
}

void SightPath1::addSight(Vector3 pos, int prevSightIdx)
{

}

void SightPath1::solve (Sight sight0, Sight sight1)
{
    Vector3 cp0 = sight0.pos, cp1 = sight1.pos, mp;
    do {
        cp0 = cp0 + sight0.tangent * CP_SCALE;
        cp1 = cp1 - sight1.tangent * CP_SCALE;
        if (intersection(sight0.pos,cp0,terrain_)) cp0 = cp0 - sight0.tangent * CP_SCALE;
        if (intersection(sight1.pos,cp1,terrain_)) cp1 = cp1 + sight1.tangent * CP_SCALE;
        mp = (cp0+cp1)/2.f;
    } while (intersection(sight0.pos,cp0,mp,terrain_) ||intersection(sight1.pos,cp1,mp,terrain_));

    controlPoints_.push_back(sight0.pos);
    controlPoints_.push_back(cp0);
    controlPoints_.push_back(mp);
    controlPoints_.push_back(cp1);
}

bool intersection(const Vector3 & v0,
		  const Vector3 &v1,
		  const Vector3 &v2,
		  const Terrain *terrain_)
{
  return intersection(v0,v1,terrain_) ||
    intersection(v0,v2,terrain_) ||
    intersection(v1,v2,terrain_);
}

bool intersection(const Vector3 & source, const Vector3 &dest, const Terrain *terrain_)
{
    std::vector<Triangle> triangles = terrain_->getTriangles(source,dest);
    Vector3 dir = (dest-source);
    Ray r(source,dir,0, 1.0f);
    for (int i = 0; i < triangles.size(); ++i) {
        IntersectionInfo ii = triangles[i].rayIntersect(r);
        //if (ii.t < dir.mag()) {
        if (ii.hit) {
	  /*printf("intersection at (%f,%f,%f)\n",ii.p.x, ii.p.y, ii.p.z);
	  printf("  source at (%f,%f,%f)\n",source.x, source.y, source.z);
	  printf("  dest at (%f,%f,%f)\n",dest.x, dest.y, dest.z);*/
	  return true;

        }
    }
    return false;
}

bool intersectionBezier(const Vector3 & p0, const Vector3 & p1, const Vector3 &p2, const Terrain *terrain_)
{
  
  float fStep = 1.f / NUM_BEZIER_COLLISION_TESTS;
  for (float f = 0.f; f < 1.f-fStep; f += fStep)
    {
      Vector3 v0 = BezierCurve::evaluate(p0,p1,p2,f);
      Vector3 v1 = BezierCurve::evaluate(p0,p1,p2,f+fStep);
      if (intersection(v0, v1, terrain_))
	return true;
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
  createMidpoints();
}

void SightPath2::createMidpoints()
{
  midpoints_.clear();
  for (unsigned int i = 0; i < numSights()-1; i++)
    {
      Vector3 m = getMidPoint(i,i+1);
      Vector3 upDirection(0,0,1);
      float midPointPushAmount = getLength(i,i+1)*MID_RAISE_FACTOR;      
      m = m + upDirection*midPointPushAmount;
      midpoints_.push_back(m);
    }
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
	      path_.push_back(midpoints_[0]);
	    }
	  else if (s.t == SiteType_Middle)
	    {
	      path_.push_back(s.p0);
	      path_.push_back(s.ctrl);
	      path_.push_back(s.p1);	      
	      path_.push_back(midpoints_[i]);
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

int SightPath2::solveFirstSegment(bool optimize)
{
  SiteSegment s;
  s.t = SiteType_First;
  s.p1 = sights_[0];
  siteSegments_[0] = s;
  return 1;
}

int SightPath2::solveLastSegment(bool optimize)
{
  SiteSegment s;
  s.t = SiteType_Last;
  s.p0 = sights_[numSights()-1];
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

void SightPath2::addSight(Vector3 pos, int prevSightIdx)
{

}
 
void SightPath2::removeSight(int index)
{
  if (index < 0 || index >= numSights())
    return;
  // update sights
  sights_.erase(sights_.begin()+index);
  // update site segments
  siteSegments_.erase(siteSegments_.begin()+index);
  // update midpoints
  if (index < numSights())
    {
      midpoints_.erase(midpoints_.begin()+index);
      if (index != 0)
	{
	  Vector3 m = getMidPoint(index-1,index);
	  Vector3 upDirection(0,0,1);
	  float midPointPushAmount = getLength(index-1,index)*MID_RAISE_FACTOR;      
	  midpoints_[index-1] = m + upDirection*midPointPushAmount;
	}
    }
  if (index == numSights()-1)
    solveSiteSegment(index);
  else
    {
      solveSiteSegment(index);
      solveSiteSegment(index+1);
    }
  isPathValid_ = 0;
}

int SightPath2::solveMidSegment(int index, bool optimize)
{
  if (index <= 0 || index >= numSights()-1)
    return 1;
  SiteSegment s;
  s.t = SiteType_Middle;
  bool doesIntersect = true;
  Vector3 cp1, p0, p1;
  float scaleFactor = START_CURVE_SCALE;
  bool hasImprovedCurvature = false;
  while (doesIntersect)
    {
      Vector3 cp0 = midpoints_[index-1]; // previous mid control point
      Vector3 cp2 = midpoints_[index]; // next mid control point
      Vector3 poi = sights_[index]; // current poi to visit
      Vector3 v1 = (poi - cp0).normalize();
      Vector3 v2 = (poi - cp2).normalize();
      Vector3 v = (v1+v2).normalize();
      cp1 = poi + v*scaleFactor; // control point for current site
      Vector3 v3 = (cp0-cp1).normalize(); // vector from cp1 back to cp0
      Vector3 v4 = (cp2-cp1).normalize(); // vector from cp1 forward to cp2
      float theta = acos(v3.dot(v4)); // angle between v3 and v4
      float L = scaleFactor / (cos(theta / 2.f)); // scale of v3 and v4 relative to v
      p0 = cp1 + v3*(2.f*L); // point between cp0 and cp1
      p1 = cp1 + v4*(2.f*L); // point between cp1 and cp2

      float curvature = BezierCurve::maxCurvature(p0,cp1,p1,50,1.f/50);
      printf("curvature at index(%i): %f\n",index,curvature);
      if (curvature > 0.15f && !hasImprovedCurvature)
	{
	  printf("bad curvature at index(%i): %f\n",index, curvature);
	  Vector3 pprev = siteSegments_[index-1].p1;
	  // calculate tangent to pprevious,cp0,p0
	  Vector3 moveDirection = (p0-pprev).normalize().cross((cp0-pprev).normalize());
	  moveDirection.normalize();
	  float part0Amount = (p0-cp0).mag() * 0.2f;
	  float part1Amount = (p1-cp1).mag() * 0.2f;
	  moveMidpoint(index-1, moveDirection * -part0Amount);
	  moveMidpoint(index, moveDirection * part1Amount);
	  scaleFactor *= 0.5f;
	  if (!hasImprovedCurvature)
	    {
	      hasImprovedCurvature = !hasImprovedCurvature;
	      continue;
	    }
	}
      // find intersections
      Vector3 prevP = siteSegments_[index-1].p1;
      int intersects1 = intersectionBezier(p0,cp1,p1,terrain_);
      int intersects2 = intersectionBezier(prevP,cp0,p0,terrain_);
      doesIntersect = intersects1 | intersects2;
      if (doesIntersect)
	{
	  printf("intersection!!! for index (%i)\n",index);
	  Vector3 moveAmount = Vector3(0,0,100.f);
	  moveMidpoint(index-1,moveAmount);
	  moveMidpoint(index,moveAmount);
	}
    }

  // store values
  s.ctrl = cp1;
  s.p0 = p0;
  s.p1 = p1;
  siteSegments_[index] = s;
  return 1;
}

// 1 on success, 0 on failure
int SightPath2::solveSiteSegment(int index, bool optimize)
{
  SiteSegment s;
  // specially handle first and last segment
  if (index == 0)
    return solveFirstSegment(optimize);
  else if (index == numSights()-1)
    return solveLastSegment(optimize);
  // handle middle segment
  else if (index > 0 && index < numSights()-1)
    return solveMidSegment(index,optimize);
  // invalid index
  else
    return -1;
  
}

// 1 on success, 0 on failure
int SightPath2::moveMidpoint(int index, Vector3 diff)
{
  if (index < 0 || index > midpoints_.size()-1)
    return 0;
  Vector3 m = midpoints_[index];
  m = m + diff;
  midpoints_[index] = m;
  printf("moving index(%i) to (%f,%f,%f)\n",index,m.x,m.y,m.z);
  // resolve current 
  solveSiteSegment(index, false);
  solveSiteSegment(index+1, false);
  isPathValid_ = 0;
}    
