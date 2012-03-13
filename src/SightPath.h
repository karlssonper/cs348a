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

#define CP0_ITER 600.0f
#define D 200.f
class Terrain;
class SightPath
{
public:
    SightPath(const Terrain * terrain,
              const std::vector<Vector3> &sights);

    void createConstraintTangents();
    void createControlPoints();
    void removeSight(int i);
    const std::vector<Vector3> & controlPoints() const { return controlPoints_;};
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
    Vector3 lineIntersect(Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3) const;
};

#endif /* SIGHTPATH_H_ */
