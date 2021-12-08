#include "gnid/hull.hpp"

#include <limits>
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
    float currentMax = -std::numeric_limits<float>::infinity();
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
