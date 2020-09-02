#include "gnid/box.hpp"

#include <cassert>
#include "gnid/matrix/matrix.hpp"

using namespace gnid;
using namespace tmat;

void Box::add(const Vector3f &v)
{
    if(count_ == 0)
    {
        min_ = v;
        max_ = v;
    }
    else
    {
        for(int i = 0; i < 3; i ++)
        {
            if(v[i] < min_[i])
                min_[i] = v[i];
            if(v[i] > max_[i])
                max_[i] = v[i];
        }
    }
    count_ ++;
}

void Box::add(const Box &box)
{
    if(count_ == 0)
    {
        min_ = box.min();
        max_ = box.max();
    }
    else
    {
        for(int i = 0; i < 3; i ++)
        {
            if(box.min()[i] < min_[i])
                min_[i] = box.min()[i];
            if(box.max()[i] > max_[i])
                max_[i] = box.max()[i];
        }
    }
    count_ ++;
}

bool Box::overlaps(const Box &other) const
{
    assert(count() > 0);
    for(int i = 0; i < 3; i ++)
    {
        if((max()[i] < other.min()[i]) || (min()[i] > other.max()[i]))
            return false;
    }
    return true;
}

bool Box::contains(const Box &other) const
{
    assert(count() > 0);
    return min() <= other.min() && max() >= other.max();
}

bool Box::contains(const Vector3f &other) const
{
    assert(count() > 0);
    return min() <= other && max() >= other;
}

Vector3f Box::center() const
{
    assert(count() > 0);
    return (min() + max()) * 0.5f;
}

Vector3f Box::size() const
{
    assert(count() > 0);
    return max() - min();
}

Vector3f Box::support(const Vector3f &d) const
{
    Vector3f ret = min();
    float retDot = d.dot(min());

    const int pointCount = 1 << 3;
    const Vector3f diff = max() - min();

    for(int i = 0; i < pointCount; i ++)
    {
        Vector3f point = min();

        /* Calculate the point's position. */
        for(int j = 0; j < 3; j ++)
        {
            if(((i >> j) & 1) == 1)
            {
                point[j] += diff[j];
            }
        }

        float dot = d.dot(point);
        if(dot > retDot)
        {
            ret = point;
            retDot = dot;
        }
    }

    return ret;
}

