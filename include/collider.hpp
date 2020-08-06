#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <memory>
#include <list>
#include <array>

#include "matrix/matrix.hpp"
#include "box.hpp"
#include "node.hpp"

namespace gnid
{

using namespace tmat;
using namespace std;

class Shape;
class Collider;

class Collision
{
public:
    /* The first collider will always be less than the second one. */
    const array<shared_ptr<Collider>, 2> &colliders() const { return colliders_; }
    const Vector3f &overlap() const { return overlap_; }

    Collision(
            const shared_ptr<Collider> a,
            const shared_ptr<Collider> b,
            Vector3f overlap)
        : colliders_ { (a < b ? a : b), (a < b ? b : a) },
          overlap_(overlap),
          visited_(true)
    {
    }

    bool operator==(const Collision &other) const
    {
        for(int i = 0; i < 2; i ++)
        {
            if(colliders_[i] != other.colliders_[i])
                return false;
        }
        return true;
    }

    bool operator<(const Collision &other) const
    {
        for(int i = 0; i < 2; i ++)
        {
            if(colliders_[i] >= other.colliders_[i])
                return false;
        }
        return true;
    }

private:
    /* Whether or not the collider has been visited this cycle. */
    array<shared_ptr<Collider>, 2> colliders_;
    mutable Vector3f overlap_;
    mutable bool visited_;

    friend class Scene;
};

class Collider : public Node
{
public:

    Collider(shared_ptr<Shape> shape) : shape_(shape)
    {
    }


    /* \brief Return the world space bounding box */
    const Box &box() const { return box_; }

    /** \brief Return the collider's shape */
    const shared_ptr<Shape> shape() const { return shape_; }

    /* \brief Called after the collider is moved to calculate its new bounding box */
    void calcBox();

    /**
     * \description Returns true if this collider is overlapping the other
     * collider. If they are overlapping, the minimum amount needed to move the
     * shapes so that they are not overlapping is stored in out.
     *
     * A bounding box check will always have been performed before this check is
     * performed, so it is not necessary to check whether their boxes overlap in
     * this function.
     */
    bool getOverlap(
            Vector3f &out,
            const shared_ptr<Collider> &other,
            const Vector3f initialAxis = Vector3f::right,
            const int maxIterations = 32) const;

    void onSceneChanged(shared_ptr<Scene> newScene) override;

    shared_ptr<Node> clone() override
    {
        return make_shared<Collider>(shape());
    }

private:
    const shared_ptr<Shape> shape_;
    Box box_;

    typedef bool (Collider::*NearestSimplexFunction)(
            array<Vector3f, 4> &s,
            int &n,
            Vector3f &d) const;

    /* Nearest simplex lookup table. */
    static NearestSimplexFunction nearestSimplexFunctions[4];

    /*
     * Takes the simplex s and transforms it to a new simplex on s closest to
     * the origin. Stores the direction toward the origin normal to the simplex
     * in d.
     *
     * Returns true if s contains the origin. Returns false otherwise.
     */
    bool nearestSimplex(array<Vector3f, 4> &s, int &n, Vector3f &d) const;

    /* Implementations for each vertex count. */
    bool nearestSimplex1(array<Vector3f, 4> &s, int &n, Vector3f &d) const;
    bool nearestSimplex2(array<Vector3f, 4> &s, int &n, Vector3f &d) const;
    bool nearestSimplex3(array<Vector3f, 4> &s, int &n, Vector3f &d) const;
    bool nearestSimplex4(array<Vector3f, 4> &s, int &n, Vector3f &d) const;

    friend class Scene;
};

} /* namespace */

#endif /* ifndef COLLIDER_HPP */
