#include "BezierCurve.h"
#include <GL/glut.h>

BezierCurve::BezierCurve(Vector3 _p1, Vector3 _p2, Vector3 _p3) 
    : p1(_p1), p2(_p2), p3(_p3) {}

Vector3 BezierCurve::evaluate(float _t) {
    return p1*(1.f-_t)*(1.f-_t) + p2*2.f*_t*(1.f-_t) + p3*_t*_t;
}

Vector3 BezierCurve::evaluate(const Vector3 &_p1, 
                              const Vector3 &_p2, 
                              const Vector3 &_p3, 
                              float _t) {
    return _p1*(1.f-_t)*(1.f-_t) + _p2*2.f*_t*(1.f-_t) + _p3*_t*_t;
}

void BezierCurve::renderCurve(const Vector3 &_p1,
                              const Vector3 &_p2,
                              const Vector3 &_p3,
                              int _steps) {
    glBegin(GL_POINTS);
    for (int i=0; i<_steps; ++i) {
        float t = (float)i/(float)_steps;
         Vector3 p = BezierCurve::evaluate(_p1, _p2, _p3, t);
         glColor3f(1,1,1);
         glVertex3f(p.x, p.y, p.z);
     }
     glEnd();
}

void BezierCurve::renderCurves(const std::vector<Vector3> &_cpts,
                               int _steps) {
    for (int i=0; i<_cpts.size()-2; i=i+2) {
        Vector3 p1 = _cpts.at(i);
        Vector3 p2 = _cpts.at(i+1);
        Vector3 p3 = _cpts.at(i+2);
        renderCurve(p1, p2, p3, _steps);
    }
}

void BezierCurve::renderCtrlPts(const std::vector<Vector3> &_cpts,
								float _size) {
	glColor3f(0.f, 1.f, 0.f);
	for (int i=0; i<_cpts.size(); i++) {
		glPushMatrix();
		glTranslatef(_cpts.at(i).x, _cpts.at(i).y, _cpts.at(i).z);
		glutSolidSphere(_size, 30, 30);
		glPopMatrix();
	}
}


