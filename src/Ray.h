#ifndef RAY_H
#define RAY_H

#include "MathEngine.h"

// e - ray's origin
// d - ray's direction

class Ray {
public:
    Ray(Vector3 _e, Vector3 _d, float _tMin, float _tMax);
    Ray(const Ray& _r);
    Vector3 e, d;
    float tMin, tMax;
};

#endif
