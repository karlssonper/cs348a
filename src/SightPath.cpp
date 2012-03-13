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
    do {
        cp0 = cp0 + sight0.tangent * CP_SCALE;
        cp1 = cp1 - sight1.tangent * CP_SCALE;
        if (intersection(sight0.pos,cp0)) cp0 = cp0 - sight0.tangent * CP_SCALE;
        if (intersection(sight1.pos,cp1)) cp1 = cp1 + sight1.tangent * CP_SCALE;
        mp = (cp0+cp1)/2.f;
    } while (intersection(sight0.pos,cp0,mp) ||intersection(sight1.pos,cp1,mp));

    controlPoints_.push_back(sight0.pos);
    controlPoints_.push_back(cp0);
    controlPoints_.push_back(mp);
    controlPoints_.push_back(cp1);
}


bool SightPath::intersection(const Vector3 & v0,
                             const Vector3 &v1,
                             const Vector3 &v2)
{
    return intersection(v0,v1) ||
            intersection(v0,v2) ||
            intersection(v1,v2);
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
            return true;

        }
    }
    return false;
}
