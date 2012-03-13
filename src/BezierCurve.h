#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "MathEngine.h"
#include <vector>

class BezierCurve {
public:
    static Vector3 evaluate(const Vector3 &_p1, 
                            const Vector3 &_p2, 
                            const Vector3 &_p3,
                            float _t);
    static void renderCurve(const Vector3 &_p1,
                            const Vector3 &_p2,
                            const Vector3 &_p3,
                            int _steps);
    static void renderCurves(const std::vector<Vector3> &_cpts,
                             int _steps);
    static void renderCtrlPts(const std::vector<Vector3> &_cpts,
							  float _size);
    BezierCurve(Vector3 _p1, Vector3 _p2, Vector3 _p3);
    Vector3 evaluate(float _t);
    Vector3 p1, p2, p3;
};

#endif
