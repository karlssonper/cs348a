#include "IntersectionInfo.h"

IntersectionInfo::IntersectionInfo(const IntersectionInfo& _i)
    : t(_i.t), p(_i.p), hit(_i.hit) {}

IntersectionInfo::IntersectionInfo(float _t, Vector3 _p, bool _hit) 
    : t(_t), p(_p), hit(_hit) {}
