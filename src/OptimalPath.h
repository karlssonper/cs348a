/*
 * SightTree.h
 *
 *  Created on: Mar 13, 2012
 *      Author: per
 */

#ifndef OPTIMALPATH_H_
#define OPTIMALPATH_H_

#include <vector>
#include <iostream>
#include <set>
#include <limits>
using namespace std;

std::vector<Vector3> cp;

typedef std::vector< std::pair<float, std::vector<int> > > testVector;

float shortest(std::set<int>& avail, int & idx)
{
    float min = std::numeric_limits<float>::max();
    int minIdx;
    for (std::set<int>::iterator it = avail.begin(); it != avail.end(); ++it){
        if (*it == idx) continue;
            float dist = (cp[idx] - cp[*it]).mag();
            if (dist < min) {
                min = dist;
                minIdx = *it;
            }
    }

    idx = minIdx;
    avail.erase(idx);
    return min;
}

std::vector<Vector3> optimalSightPath(const std::vector<Vector3>& _cp)
{
    cp = _cp;
    testVector t(cp.size());
    for (int i = 0; i < t.size(); ++i) {

        std::set<int> avail;
        for (int j = 0; j < t.size(); ++j) avail.insert(j);
        avail.erase(i);

        std::vector<int> path;
        float length = 0.f;
        int idx = i;
        path.push_back(idx);
        for (int j = 0; j < t.size() -1; ++j){
            length += shortest(avail,idx);
            path.push_back(idx);
        }

        t[i].first = length;
        t[i].second = path;
    }

    float min = std::numeric_limits<float>::max();
    int minIdx;
    for (int j = 0; j < t.size(); ++j) {
        if (t[j].first < min) {
            min = t[j].first;
            minIdx = j;
        }
    }

    std::vector<Vector3> newCP(cp.size());
    for (int j = 0; j < cp.size(); ++j) {
        newCP[j] = cp[t[minIdx].second[j]];
        newCP[j].z += 200;
        printf("%f %f %f\n", newCP[j].x,newCP[j].y,newCP[j].z);
    }
    int i;
    std::cin >> i;
    return newCP;

}

//class
#endif /* SIGHTTREE_H_ */
