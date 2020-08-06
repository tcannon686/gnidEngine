#ifndef BOX_HPP
#define BOX_HPP

#include <cassert>
#include "shape.hpp"
#include "matrix/matrix.hpp"

namespace gnid
{

using namespace tmat;

class Box : public Shape
{
public:
    Box() {}

    /* Adds the point to the box and returns the box for convenience. */
    void add(const Vector3f v);

    /* Adds the given box to the box and returns this box for convenience. */
    void add(const Box other);
    bool overlaps(const Box other) const;
    bool contains(const Box other) const;
    bool contains(const Vector3f other) const;

    /* Transform the box by the given matrix, resulting in a new box. */
    void transform(Matrix4f matrix);

    const Vector3f &min() const { assert(count() > 0); return min_; }
    const Vector3f &max() const { assert(count() > 0); return max_; }
    Vector3f center() const;
    const int &count() const { return count_; }

    void clear() { count_ = 0; }

    /******** Collider shape methods. ********/
    const Box &box() const override;
    Vector3f support(const Vector3f d) const override;

private:
    Vector3f min_;
    Vector3f max_;
    int count_ = 0;
};

} /* namespace */

#endif /* ifndef BOX_HPP */

