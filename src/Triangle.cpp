#include "Triangle.h"
#include "IntersectionInfo.h"
#include "Ray.h"

Triangle::Triangle(Vector3 _v1, Vector3 _v2, Vector3 _v3) 
    : v1(_v1), v2(_v2), v3(_v3) {
    Vector3 nTemp = _v1.cross(_v2);
    n = nTemp.normalize();
}

Triangle::Triangle(const Triangle &t)
    : v1(_t.v1), v2(_t.v2), v3(_t.v3), n(_t.n) {}

IntersectionInfo Triangle::rayIntersect(Ray _ray) {

    float a = v1.x - v2.x;
    float b = v1.y - v2.y;
    float c = v1.z - v2.z;
    float d = v1.x - v3.x;
    float e = v1.y - v3.y;
    float f = v1.z - v3.z;
    float g = _ray.d.x;
    float h = _ray.d.y;
    float i = _ray.d.z;
    float j = v1.x - _ray.e.x;
    float k = v1.y - _ray.e.y;
    float l = v1.z - _ray.e.z;
    float M = a*(e*i-h*f)+b*(g*f-d*i)+c*(d*h-e*g);
    float t = (-1.f)*(f*(a*k-j*b)+e*(j*c-a*l)+d*(b*l-k*c))/M;
    if ( t<_ray.tMin || t>_ray.tMax ) {

        // miss!
        return IntersectionInfo( 0.f,
                                 Vector3(), 
                                 false );

    }
    float gamma = (i*(a*k-j*b)+h*(j*c-a*l)+g*(b*l-k*c))/M;
    if (gamma < 0.f || gamma > 1.f) {

        // miss!
        return IntersectionInfo( 0.f,
                                 Vector3(), 
                                 false );
    }
    float beta = (j*(e*i-h*f)+k*(g*f-d*i)+l*(d*h-e*g))/M;
    if (beta < 0.f || beta > (1.f - gamma)) {

        // miss!
        return IntersectionInfo( 0.f,
                                 Vector3(), 
                                 false );
    }
    
    // hit!
    return IntersectionInfo( t,
                             Vector3(_ray.e+_ray.d*t),
                             true );

}



