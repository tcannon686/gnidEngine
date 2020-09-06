#include "gnid/sphere.hpp"

using namespace gnid;
using namespace tmat;

Sphere::Sphere(float radius)
    : radius_(radius)
{
}

Vector3f Sphere::support(const Vector3f &d) const
{
    return d.normalized() * radius();
}

