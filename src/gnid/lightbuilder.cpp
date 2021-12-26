#include "gnid/lightbuilder.hpp"
#include "gnid/spatialnode.hpp"
#include "gnid/directionallight.hpp"
#include "gnid/ambientlight.hpp"
#include "gnid/pointlight.hpp"

#include <cassert>

using namespace std;
using namespace gnid;

LightBuilder &LightBuilder::direction(const tmat::Vector3f &direction)
{
    direction_ = direction;
    isDirectional_ = true;
    return *this;
}

LightBuilder &LightBuilder::point(const tmat::Vector3f &point)
{
    point_ = point;
    isPoint_ = true;
    return *this;
}

LightBuilder &LightBuilder::ambient()
{
    isAmbient_ = true;
    return *this;
}

LightBuilder &LightBuilder::color(float r, float g, float b)
{
    color_ = { r, g, b };
    return *this;
}

LightBuilder &LightBuilder::color(const tmat::Vector3f &color)
{
    color_ = color;
    return *this;
}

LightBuilder &LightBuilder::distance(float distance)
{
    assert(isPoint_);
    distance_ = distance;
    return *this;
}

shared_ptr<Node> LightBuilder::build()
{
    assert(isDirectional_ + isPoint_ + isAmbient_ == 1);

    if(isDirectional_)
    {
        const auto light = std::make_shared<DirectionalLight>();
        light->direction() = direction_;
        light->color() = color_;
        return light;
    }
    else if(isPoint_)
    {
        const auto spatial = std::make_shared<SpatialNode>();
        const auto light = std::make_shared<PointLight>();
        spatial->add(light);
        light->color() = color_;
        light->distance() = distance_;
        return spatial;
    }
    else
    {
        assert(isAmbient_);
        const auto light = std::make_shared<AmbientLight>();
        light->color() = color_;
        return light;
    }
}
