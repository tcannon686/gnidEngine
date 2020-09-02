#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "gnid/matrix/matrix.hpp"

namespace gnid
{

class Box;

/**
 * \brief A collision shape primitive for use with GJK and EPA
 */
class Shape
{
public:
     /**
      * \brief Returns the point on the shape with the highest dot product with d
      */
     virtual tmat::Vector3f support(const tmat::Vector3f &d) const = 0;
};

} /* namespace */

#endif
