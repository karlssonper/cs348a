/*
 * SightPath.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: per
 */

#include "SightPath.h"

SightPath::SightPath(const std::vector<Vector3> & pos,
          const std::vector<int> &idx,
          const std::vector<Vector3> &sights)
{
    sights_.resize(sights.size());
    for (int i = 0; i < sights.size(); ++i) {
        sights_[i].pos = sights[i];
    }
}

Vector3 SightPath::tangent(Vector3 p0, Vector3 p1)
{
    Vector3 dir = p1 - p0;
#define PI 3.14159265
    float phi = atan2(dir.y, dir.x)* 180 / PI;
    float theta = 135;
    Vector3 t(cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta));
    if (t.mag() == 0.0f) std::cerr << "Error tangents!!";
    t = t/t.mag();
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
        scale /= 2.f;
        //hit = intersection(sight0.pos,cp0);
    } while (hit);

    Vector3 mp = (cp0 + sight1.pos)/2.0f;
    Vector3 u = cp0 - sight1.pos;
    u = u/u.mag();
    Vector3 mp_dir = sight1.tangent - (u*u.dot(sight1.tangent));
    mp_dir = mp_dir / mp_dir.mag();
    hit = true;
    do {
        mp = mp + mp_dir;
        //hit = intersection(cp0,mp);
    } while (hit);

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
}

Vector3 SightPath::lineIntersect(Vector3 v0,
                                          Vector3 v1,
                                          Vector3 v2,
                                          Vector3 v3) const
{
    float den = v3.x*v1.y - v3.y*v1.x;
    if (den == 0.0f) std::cerr << "AAAH LINEINTERSECT ERROR!!";
    float t2 = (v1.x*(v2.y-v0.y) - v1.y * (v2.x-v0.x)/den);
    return v2 + v3*t2;
}
