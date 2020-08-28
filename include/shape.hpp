#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "matrix/matrix.hpp"

namespace gnid
{

using namespace tmat;

class Box;

/**
 * \brief A collision shape primitive for use with GJK and EPA
 */
class Shape
{
public:
     /**
      * \brief Returns the bounding box for the shape
      */
     virtual const Box &box() const = 0;
     /**
      * \brief Returns the point on the shape with the highest dot product with d
      */
     virtual Vector3f support(const Vector3f d) const = 0;
};

} /* namespace */

#endif
