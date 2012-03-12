/*
 * SightPath.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: per
 */

#include "SightPath.h"

SightPath::SightPath(const std::vector<float> & pos,
          const std::vector<int> &idx,
          const std::vector<float> &sights)
{

}

void SightPath::createConstraintTangents()
{
    //first point's tangent is the direction to the first sight
    Vector3 firstTangent = sights_[1].pos - sights_[0].pos;
    if (firstTangent.mag() == 0.0f) std::cerr << "Error tangents!!";
    sights_.front().tangent = firstTangent / firstTangent.mag();

    for (int i = 1; i < sights_.size() - 1; ++i) {
        Vector3 tangent = sights_[i+1].pos - sights_[i-1].pos;
        if (tangent.mag() == 0.0f) std::cerr << "Error tangents!!";
        sights_[i].tangent = tangent / tangent.mag();
    }

    //last point's tangent is direction of the second last point.
    Vector3 lastTangent = sights_[sights_.size()-1].pos -
                          sights_[sights_.size()-2].pos;
    if (lastTangent.mag() == 0.0f) std::cerr << "Error tangents!!";
    sights_.back().tangent = lastTangent / lastTangent.mag();
}

void SightPath::createControlPoints()
{

}
void SightPath::solve (Sight sight0, Sight sight1)
{

}
