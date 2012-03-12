#ifndef INTERSECTIONINFO_H
#define INTERSECTIONINFO_H
 
// t - t value of intersection (garbage if miss)
// p - point of intersection (garbage if miss)
// hit - true if hit, false if miss
 
class IntersectionInfo {
    IntersectionInfo(float _t, Vector3 _p, bool _hit);
public:
    float t;
    Vector3 p;
    bool hit;
};
