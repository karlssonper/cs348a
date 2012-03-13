#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "MathEngine.h"

class BezierCurve {
public:
    static Vector3 evaluate(Vector3 _p1, 
                            Vector3 _p2, 
                            Vector3 _p3,
                            float _t);
    BezierCurve(Vector3 _p1, Vector3 _p2, Vector3 _p3);
    Vector3 evaluate(float _t);
    Vector3 p1, p2, p3;
};

#endif
