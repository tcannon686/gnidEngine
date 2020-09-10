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
    Vector3f ret;
    for(int i = 0; i < 3; i ++)
    {
        if(d[i] == 0)
            ret[i] = 0;
        else
            ret[i] = d[i] > 0 ? max_[i] : min_[i];
    }
    return ret;
}

