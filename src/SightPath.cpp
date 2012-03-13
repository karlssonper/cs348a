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
    Vector3 cp0;
    bool hit = true;
    float scale = 1.f;
    do {
        cp0 = sight0.pos + sight0.tangent * CP0_ITER/scale;
        Vector3 test = sight0.tangent * CP0_ITER/scale;
        std::cout << "cp0 tanget: " << test.x << " " << test.y << " " << test.z << std::endl;
        std::cout << "cp0 mag: " << test.mag() << std::endl;
        scale *= 2.f;
        hit = intersection(sight0.pos,cp0);
    } while (hit);

    //std::cout << "First hit done" << std::endl;

    Vector3 mp = (cp0 + sight1.pos)/2.0f;
    Vector3 u = cp0 - sight1.pos;
    u = u/u.mag();
    Vector3 mp_dir = sight1.tangent*-1.f - (u*u.dot(sight1.tangent*-1.f));
    mp_dir = mp_dir / mp_dir.mag();
    hit = true;
    do {
        mp = mp + mp_dir;
        //std::cout << "Before first intersection hit done" << std::endl;
        //std::cout << "mp pos: " << mp.x << " " << mp.y << " " << mp.z << std::endl;

        hit = intersection(cp0,mp);
    } while (hit);
    //std::cout << "Second hit done" << std::endl;

    Vector3 v0 = cp0;
    Vector3 v1 = mp-cp0;
    v1 = v1/v1.mag();
    Vector3 v2 = sight1.pos;
    Vector3 v3 = sight1.tangent * (-1.f);

    Vector3 cp1 = lineIntersect(v0,v1,v2,v3);

    controlPoints_.push_back(sight0.pos);
    controlPoints_.push_back(cp0);
    controlPoints_.push_back(mp);
    controlPoints_.push_back(cp1);
    printf("Point %f %f %f\n", sight0.pos.x, sight0.pos.y, sight0.pos.z);
    printf("Point %f %f %f\n", cp0.x, cp0.y, cp0.z);
    printf("Point %f %f %f\n", mp.x, mp.y, mp.z);
    printf("Point %f %f %f\n", cp1.x, cp1.y, cp1.z);

    std::cout << "4 new Control points added!" << std::endl;
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
