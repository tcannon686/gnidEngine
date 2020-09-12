#include "gnid/hull.hpp"

#include <vector>

#include "gnid/matrix/matrix.hpp"

using namespace gnid;
using namespace std;
using namespace tmat;

Hull::Hull(const vector<Vector3f> &points)
    : points_(points)
{
}

Vector3f Hull::support(const tmat::Vector3f &d) const
{
    float currentMax = 0.0f;
    Vector3f currentPoint;

    for(auto &point : points_)
    {
        float dot = d.dot(point);
        if(dot >= currentMax)
        {
            currentMax = dot;
            currentPoint = point;
        }
    }

    return currentPoint;
}
