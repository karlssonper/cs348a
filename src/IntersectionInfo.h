#ifndef INTERSECTIONINFO_H
#define INTERSECTIONINFO_H

#include "MathEngine.h"
 
// t - t value of intersection (garbage if miss)
// p - point of intersection (garbage if miss)
// hit - true if hit, false if miss
 
class IntersectionInfo {
public: 
    IntersectionInfo(float _t, Vector3 _p, bool _hit);
    IntersectionInfo(const IntersectionInfo& _i);
    float t;
    Vector3 p;
    bool hit;
};

#endif
