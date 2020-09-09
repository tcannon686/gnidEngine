#include "gnid/capsule.hpp"

using namespace std;
using namespace tmat;
using namespace gnid;

Capsule::Capsule(float radius, Vector3f start, Vector3f end)
    : radius_(radius),
      start_(start),
      end_(end),
      d_(end_ - start_)
{
}

Vector3f Capsule::support(const tmat::Vector3f &d) const
{
    if(d_.dot(d) > 0)
    {
        return end_ + d.normalized() * radius_;
    }
    else
    {
        return start_ + d.normalized() * radius_;
    }
}
