#include "BezierCurve.h"

BezierCurve::BezierCurve(Vector3 _p1, Vector3 _p2, Vector3 _p3) 
    : p1(_p1), p2(_p2), p3(_p3) {}

Vector3 BezierCurve::evaluate(float _t) {
    return p1*(1.f-_t*_t) + p2*2.f*_t*(1.f-_t) + p3*_t*_t;
}

Vector3 BezierCurve::evaluate(Vector3 _p1, Vector3 _p2, Vector3 _p3, float _t) {
    return _p1*(1.f-_t*_t) + _p2*2.f*_t*(1.f-_t) + _p3*_t*_t;
}





