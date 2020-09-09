#ifndef CAPSULE_HPP
#define CAPSULE_HPP

#include "gnid/matrix/matrix.hpp"
#include "gnid/shape.hpp"

namespace gnid
{

/**
 * \brief A capsule (cylinder with round ends) shape
 */
class Capsule : public gnid::Shape
{
public:
    Capsule(float radius, tmat::Vector3f start, tmat::Vector3f end);

    tmat::Vector3f support(const tmat::Vector3f &d) const override;

    const float &radius() const { return radius_; }
    const tmat::Vector3f &start() const { return start_; }
    const tmat::Vector3f &end() const { return end_; }

private:
    float radius_;
    tmat::Vector3f start_;
    tmat::Vector3f end_;
    tmat::Vector3f d_;
};

} /* namespace */

#endif
