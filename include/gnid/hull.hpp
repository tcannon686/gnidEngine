#ifndef HULL_HPP
#define HULL_HPP

#include <vector>
#include <cassert>

#include "gnid/matrix/matrix.hpp"
#include "gnid/shape.hpp"

namespace gnid
{

/**
 * \brief A convex hull shape made of a polyhedron
 *
 * \todo
 *      - Implement faster algorithm, currently just does a simple O(n) search
 *        on the points.
 */
class Hull : public Shape
{
public:
    Hull(const std::vector<tmat::Vector3f> &points);

    tmat::Vector3f support(const tmat::Vector3f &d) const override;

private:
    const std::vector<tmat::Vector3f> points_;
};

} /* namespace */

#endif
