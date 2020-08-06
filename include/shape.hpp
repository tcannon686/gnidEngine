#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "matrix/matrix.hpp"

namespace gnid
{

using namespace tmat;

class Box;

class Shape
{
public:
     /* Return the bounding box for the shape. */
     virtual const Box &box() const = 0;
     /* Return the point on the shape with the highest dot product with d. */
     virtual Vector3f support(const Vector3f d) const = 0;
};

} /* namespace */

#endif
