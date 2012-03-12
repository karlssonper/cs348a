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

struct Sight {
    Vector3 pos;
    Vector3 tangent;
};

class SightPath
{
public:
    SightPath(const std::vector<Vector3> & pos,
              const std::vector<int> &idx,
              const std::vector<Vector3> &sights);

    void createConstraintTangents();
    void createControlPoints();
    const std::vector<Vector3> & controlPoints() const { return controlPoints_;};
private:
    std::vector<Sight> sights_;
    std::vector<Vector3> controlPoints_;

    void solve (Sight sight0, Sight sight1);
};

#endif /* SIGHTPATH_H_ */
