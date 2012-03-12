include "Ray.h"

Ray::Ray(Vector3 _e, Vector3 _d, float _tMin, float _tMax)
    : e(_e), d(_d), tMin(_tMin), tMax(_tMax) {}

Ray::Ray(const Ray& _r) 
    : e(_r.e), d(_r.d), tMin(_r.tMin), tMax(_r.tMax) {}
