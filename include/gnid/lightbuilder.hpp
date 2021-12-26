#ifndef LIGHTBUILDER_HPP
#define LIGHTBUILDER_HPP

#include "matrix/matrix.hpp"
#include "gnid/node.hpp"

namespace gnid
{

class LightBuilder
{
public:
    LightBuilder &direction(const tmat::Vector3f &direction);
    LightBuilder &point(const tmat::Vector3f &point);
    LightBuilder &ambient();
    LightBuilder &color(const tmat::Vector3f &color);
    LightBuilder &color(float r, float g, float b);
    LightBuilder &distance(float distance);
    std::shared_ptr<Node> build();

private:
    tmat::Vector3f color_ { 1.0f, 1.0f, 1.0f };
    tmat::Vector3f direction_ = tmat::Vector3f::up;
    tmat::Vector3f point_ = tmat::Vector3f::up;
    float distance_ = 1.0f;
    bool isDirectional_ = false;
    bool isPoint_ = false;
    bool isAmbient_ = false;
};

} /* namespace */

#endif
