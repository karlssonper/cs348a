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
                              int _steps,
							  float _lineWidth) {
    glLineWidth(_lineWidth);
    glBegin(GL_LINE_STRIP);
    glColor3f(1,1,1);
    for (int i=0; i<_steps; ++i) {
        float t = (float)i/(float)(_steps-1);
         Vector3 p = BezierCurve::evaluate(_p1, _p2, _p3, t);
         glVertex3f(p.x, p.y, p.z);
     }
     glEnd();
}

void BezierCurve::renderCurves(const std::vector<Vector3> &_cpts,
                               int _steps,
							   float _lineWidth) {
    for (int i=0; i<_cpts.size()-2; i=i+2) {
        Vector3 p1 = _cpts.at(i);
        Vector3 p2 = _cpts.at(i+1);
        Vector3 p3 = _cpts.at(i+2);
        renderCurve(p1, p2, p3, _steps, _lineWidth);
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

void BezierCurve::renderCtrlPoly(const std::vector<Vector3> &_cpts,
								 float _lineWidth) {
	glLineWidth(_lineWidth);
	glColor3f(1.f, 1.f, 0.f);
	glBegin(GL_LINE_STRIP);
	for (int i=0; i<_cpts.size(); i++) {
		glVertex3f(_cpts.at(i).x, _cpts.at(i).y, _cpts.at(i).z);
	}
	glEnd();
}

void BezierCurve::renderCtrlPolyExt(const std::vector<Vector3> &_cpts,
									float _lineWidth) {
	glLineWidth(_lineWidth);
	glColor3f(1.f, 0.5f, 0.f);
	glBegin(GL_LINES);
	for (int i=2; i<_cpts.size(); i=i+2) {
		Vector3 dir = _cpts.at(i)-_cpts.at(i-1);
		Vector3 ext = _cpts.at(i) + dir;
		glVertex3f(_cpts.at(i).x, _cpts.at(i).y, _cpts.at(i).z);
		glVertex3f(ext.x, ext.y, ext.z);
	}
	glEnd();
}

void BezierCurve::renderCtrlPolyFill(const std::vector<Vector3> &_cpts){
	
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 0.4f, 0.4f, 0.6f);
	glBegin(GL_TRIANGLES);
	for (int i=0; i<_cpts.size()-2; i=i+2) {
        Vector3 p1 = _cpts.at(i);
        Vector3 p2 = _cpts.at(i+1);
        Vector3 p3 = _cpts.at(i+2);
        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
    }
    glEnd();
	


}
