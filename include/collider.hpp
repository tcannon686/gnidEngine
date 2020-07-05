#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <memory>
#include "matrix/matrix.hpp"
#include "box.hpp"

namespace gnid
{

/* 
 * Collider node using the given shape class. Shape must be a class with the
 * following methods:
 *
 * // Return the bounding box for the shape.
 * const Box3f &box() const;
 *
 * // Return the point on the shape with the highest dot product with d.
 * Vector3f support(const Vector3f d) const;
 *
 */
template<typename Shape>
class Collider : public Node
{
public:

    Collider(Shape shape) : shape_(shape)
    {
    }

    /* Return the world space bounding box. */
    const Box<3, float> &box() const { return box_; }

    /* Return the collider's shape. */
    const Shape shape() const { return shape_; }

    /* Called after the collider is moved to calculate its new bounding box. */
    void calcBox();

    /*
     * Returns true if this collider is overlapping the other collider. If they
     * are overlapping, the minimum amount needed to move the shapes so that
     * they are not overlapping is stored in out.
     *
     * A bounding box check will always have been performed before this check is
     * performed, so it is not necessary to check whether their boxes overlap in
     * this function.
     */
    template<class Shape2>
    bool getOverlap(
            Vector3f &out,
            const shared_ptr<Collider<Shape2>> &other,
            const Vector3f initialAxis = Vector3f::right) const;

    shared_ptr<Node> clone() override
    {
        return make_shared<Collider<Shape>>(shape());
    }

private:
    const Shape shape_;
    Box<3, float> box_;

    /*
     * Takes the simplex s and transforms it to a new simplex on s closest to
     * the origin. Stores the direction toward the origin normal to the simplex
     * in d.
     *
     * Returns true if s contains the origin. Returns false otherwise.
     */
    bool nearestSimplex(Vector3f s[], int &n, Vector3f &d);
};

template<typename Shape>
void Collider<Shape>::calcBox()
{
    box_ = shape().box();
    box_.transform(getWorldMatrix());
}


template<typename Shape>
bool Collider<Shape>::nearestSimplex(Vector3f s[], int &n, Vector3f &d)
{
}

template<class Shape1, class Shape2>
bool Collider<Shape1>::getOverlap(
        Vector3f &out,
        const shared_ptr<Collider<Shape2>> &other,
        const Vector3f initialAxis = Vector3f::right) const
{
    // TODO apply transformation.
    Vector3f a = shape().support(initialAxis)
        - other->shape().support(-initialAxis);

    Vector3f s[4];
    int n = 1;
    s[0] = a;

    Vector3f d = -a;

    while(true)
    {
        a = shape().support(d) - other->shape.support(-d);

        if(a.dot(b) < 0)
            return false;

        /* Add to the simplex. */
        s[n ++] = a;

        if(nearestSimplex(s, n, d))
        {
            return true;
        }
    }
}

} /* namespace */

#endif /* ifndef COLLIDER_HPP */
