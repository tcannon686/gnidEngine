#include "gnid/pointlight.hpp"

using namespace std;
using namespace gnid;

PointLight::PointLight()
{
}

float &PointLight::distance()
{
    return distance_;
}

shared_ptr<Node> PointLight::clone()
{
    auto ret = std::make_shared<PointLight>(*this);
    ret->cloneChildren(shared_from_this());
    return ret;
}

