#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "gnid/matrix/matrix.hpp"
#include "gnid/shape.hpp"

namespace gnid
{

/**
 * \brief A sphere shape
 */
class Sphere : public Shape
{
public:
    Sphere(float radius = 1.0f);

    tmat::Vector3f support(const tmat::Vector3f &d) const override;

    const float &radius() const { return radius_; }

private:
    float radius_;
};

} /* namespace */

#endif
