#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "MathEngine.h"
 
class Ray;
class IntersectionInfo;
 
class Triangle {
public:
    Triangle(Vector3 _v1, Vector3 _v2, Vector3 _v3);
    IntersectionInfo rayIntersect(Ray _ray);
    Vector3 v1, v2, v3, n;
};
 
#endif
