#ifndef BOX_HPP
#define BOX_HPP

#include <cassert>
#include "shape.hpp"
#include "matrix/matrix.hpp"

namespace gnid
{

/**
 * \brief A bounding box
 */
class Box : public Shape
{
public:
    Box() {}

    /**
     * \brief Adds the point to the box
     *
     * \details
     *     If no items have been added to the box, the box becomes a box of size
     *     zero focused on the given point. Otherwise, the box is expanded if
     *     necessary to include the point.
     */
    void add(const tmat::Vector3f &v);

    /**
     * \brief Adds the points of the given box to this box
     *
     * \details
     *     If no items have been added to this box, it becomes the other box.
     *     Otherwise, the box is expanded so that all of the points in the given
     *     box also fit in this box.
     */
    void add(const Box &other);

    /**
     * \brief Returns true if this box overlaps the given box
     */
    bool overlaps(const Box &other) const;

    /**
     * \brief Returns true if this box entirely contains the given box
     */
    bool contains(const Box &other) const;

    /**
     * \brief Returns true if the given vector is contained within the box
     */
    bool contains(const tmat::Vector3f &other) const;

    /**
     * \brief Returns the lower corner of the box
     */
    const tmat::Vector3f &min() const { assert(count() > 0); return min_; }

    /**
     * \brief Returns the upper corner of the box
     */
    const tmat::Vector3f &max() const { assert(count() > 0); return max_; }

    /**
     * \brief Calculates the center of the box and returns it
     */
    tmat::Vector3f center() const;

    /**
     * \brief Calculates the size of the box and returns it
     */
    tmat::Vector3f size() const;

    /**
     * \brief Returns the number of points added to the box
     */
    const int &count() const { return count_; }

    /**
     * \brief Clears all points from the box
     */
    void clear() { count_ = 0; }

    /******** Collider shape methods. ********/
    tmat::Vector3f support(const tmat::Vector3f &d) const override;

private:
    tmat::Vector3f min_;
    tmat::Vector3f max_;
    int count_ = 0;
};

} /* namespace */

#endif /* ifndef BOX_HPP */

