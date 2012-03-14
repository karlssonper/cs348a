#include "BezierCurve.h"
#include <GL/glut.h>
#include "limits"
#include "Triangle.h"
#include "Ray.h"
#include "IntersectionInfo.h"

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

Vector3 BezierCurve::evaluateGlobal(const std::vector<Vector3> &_cpts,
								    float _t) {
										
	// figure out how many distinct curves we have
	int nrCurves = (_cpts.size()-1)/2;
	
	// figure out how much of [0,1] each curve represents
	float localInterval = 1.f / (float)nrCurves;
	
	// figure out which local control polygon we are in
	int localPolygon = floor(_t / localInterval);
	
	// roundoff issues towards the end (just return last possible point)
	if (2*localPolygon+2 > _cpts.size()-1) {
		Vector3 p1 = _cpts.at(_cpts.size()-3);
		Vector3 p2 = _cpts.at(_cpts.size()-2);
		Vector3 p3 = _cpts.at(_cpts.size()-1);
		return evaluate(p1, p2, p3, 1.f);
	}
	
	// choose the three control points for the local polygon
	Vector3 p1 = _cpts.at(2*localPolygon);
	Vector3 p2 = _cpts.at(2*localPolygon+1);
	Vector3 p3 = _cpts.at(2*localPolygon+2);
	
	// translate the global interval to [0,1] locally and evaluate
	float tLocal = (_t-localInterval*localPolygon)/localInterval;
	return evaluate(p1, p2, p3, tLocal);									
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
	glColor3f(0.f, 0.7f, 0.f);
	for (int i=0; i<_cpts.size(); i++) {
		glPushMatrix();
		glTranslatef(_cpts.at(i).x, _cpts.at(i).y, _cpts.at(i).z);
		glutSolidCube(_size);
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
	glColor3f(0.f, 0.3f, 0.7f);
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
	glColor4f(0.7f, 0.0f, 0.0f, 0.3f);
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

float BezierCurve::length(const Vector3 &_p1,
						  const Vector3 &_p2,
						  const Vector3 &_p3,
						  int _steps) {
	float l = 0.f;
    for (int i=1; i<_steps; ++i) {
		float t0 = (float)(i-1)/(float)(_steps-1);
        float t1 = (float)i/(float)(_steps-1);
        Vector3 p0 = BezierCurve::evaluate(_p1, _p2, _p3, t0);
        Vector3 p1 = BezierCurve::evaluate(_p1, _p2, _p3, t1);
        l += (p1-p0).mag();
     }	
     std::cout << "l = " << l << std::endl;
     return l;
}

float BezierCurve::length(const std::vector<Vector3> &_cpts, 
                          int _steps) {
	float l = 0.f;
	for (int i=0; i<_cpts.size()-2; i=i+2) {
		Vector3 p1 = _cpts.at(i);
		Vector3 p2 = _cpts.at(i+1);
		Vector3 p3 = _cpts.at(i+2);
		l += length(p1, p2, p3, _steps);
	}
	return l;
}

float BezierCurve::curvature(const Vector3 &_p1,
						     const Vector3 &_p2,
						     const Vector3 &_p3,
						     float _t) {
								 
								 
}

float BezierCurve::minDistance(const Vector3 &_p1,
							   const Vector3 &_p2,
							   const Vector3 &_p3,
							   int _steps,
							   const Terrain *_terrain) {
	float min = std::numeric_limits<float>::max();	
	for (int i=0; i<_steps; ++i) {
        float t = (float)i/(float)(_steps-1);
		Vector3 p = evaluate(_p1, _p2, _p3, t);
		std::vector<Triangle> triangles = _terrain->getTriangles(p,p);
		Vector3 dir = Vector3(0.f, 0.f, -1.f);
		dir.normalize();
		Ray r(p, dir, 0, 1.0f);
		for (int i = 0; i < triangles.size(); ++i) {
			IntersectionInfo ii = triangles[i].rayIntersect(r);
			if (ii.hit) {
				if (ii.t < min) {
					min = ii.t;
				}
			}
		}
    }
    return min;							
}

float BezierCurve::minDistance(const std::vector<Vector3> &_cpts,
							   int _steps,
							   const Terrain *_terrain) {
	float min = std::numeric_limits<float>::max();					   
	for (int i=0; i<_cpts.size()-2; i=i+2) {
        Vector3 p1 = _cpts.at(i);
        Vector3 p2 = _cpts.at(i+1);
        Vector3 p3 = _cpts.at(i+2);
        float temp = minDistance(p1, p2, p3, _steps, _terrain);
        if (temp < min) min = temp;
    }
    return min;							   
}
