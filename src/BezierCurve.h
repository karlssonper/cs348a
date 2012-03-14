#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include "MathEngine.h"
#include <vector>
#include "terrain.h"

class BezierCurve {
public:
    static Vector3 evaluate(const Vector3 &_p1, 
                            const Vector3 &_p2, 
                            const Vector3 &_p3,
                            float _t);
	static Vector3 evaluateGlobal(const std::vector<Vector3> &_cpts,
								  float _t);
    static void renderCurve(const Vector3 &_p1,
                            const Vector3 &_p2,
                            const Vector3 &_p3,
                            int _steps,
                            float _lineWidth);
    static void renderCurves(const std::vector<Vector3> &_cpts,
                             int _steps,
							 float _lineWidth);
    static void renderCtrlPts(const std::vector<Vector3> &_cpts,
							  float _size);
	static void renderCtrlPoly(const std::vector<Vector3> &_cpts,
							   float _lineWidth);
	static void renderCtrlPolyExt(const std::vector<Vector3> &_cpts,
								  float _lineWidth);
	static void renderCtrlPolyFill(const std::vector<Vector3> &_cpts);
	
	static float length(const Vector3 &_p1,
						const Vector3 &_p2,
						const Vector3 &_p3,
						int _steps);
	static float length(const std::vector<Vector3> &_cpts, int _steps);
	
	static float curvature(const Vector3 &_p1,
						   const Vector3 &_p2,
						   const Vector3 &_p3,
						   float _t,
						   float _dt);

						   
	static float minDistance(const Vector3 &_p1,
						     const Vector3 &_p2,
						     const Vector3 &_p3,
						     int _steps,
						     const Terrain *_terrain);
						  
	static float minDistance(const std::vector<Vector3> &_cpts,
							 int _steps,
							 const Terrain *_terrain);
	
    BezierCurve(Vector3 _p1, Vector3 _p2, Vector3 _p3);
    Vector3 evaluate(float _t);
    Vector3 p1, p2, p3;
};

#endif
