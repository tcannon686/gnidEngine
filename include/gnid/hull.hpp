#ifndef HULL_HPP
#define HULL_HPP

#include <vector>
#include <cassert>

#include "gnid/shape.hpp"

namespace gnid
{

/**
 * \brief A convex hull shape made of a polyhedron (currently in progress)
 *
 * \todo
 *      - What data structure should be used to store the points?
 *      - Implement all features
 */
class Hull : public Shape
{
public:
    Hull(const std::vector<tmat::Vector3f> &points)
    {
        /* TODO implement */
        assert(false);
    }

    tmat::Vector3f support(const tmat::Vector3f &d) override
    {
        /* TODO implement */
        assert(false);
    }
};

} /* namespace */

#endif
