#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <memory>
#include <list>
#include <vector>

#include "matrix/matrix.hpp"
#include "box.hpp"
#include "node.hpp"

namespace gnid
{

class Shape;
class Collider;

/**
 * \brief A node capable of collision
 */
class Collision
{
public:
    /**
     * \brief A collision between two colliders
     *
     * \details
     *     The first collider pointer will always be less than the second one.
     */
    const std::array<std::shared_ptr<Collider>, 2> &colliders() const
    { return colliders_; }

    /**
     * \brief The overlap between the two
     */
    const tmat::Vector3f &overlap() const { return overlap_; }

    Collision(
            const std::shared_ptr<Collider> a,
            const std::shared_ptr<Collider> b,
            tmat::Vector3f overlap)
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
    std::array<std::shared_ptr<Collider>, 2> colliders_;
    mutable tmat::Vector3f overlap_;

    /* Whether or not the collider has been visited this cycle. */
    mutable bool visited_;

    friend class Scene;
};

/**
 * \brief A node capable of colliding with other shapes
 *
 * \details
 *     This node should be the descendant of a Rigidbody node.
 */
class Collider : public Node
{
public:

    /**
     * \brief Create a collider from the given shape
     */
    Collider(std::shared_ptr<Shape> shape) : shape_(shape)
    {
    }

    /**
     * \brief Return the world space bounding box
     */
    const Box &box() const { return box_; }

    /**
     * \brief Return the collider's shape
     */
    const std::shared_ptr<Shape> shape() const { return shape_; }

    /**
     * \brief
     *     Called after the collider is moved to calculate its new bounding box
     */
    void calcBox();

    /**
     * \brief Return the overlap between this collider and another one
     *
     * \description
     *     Returns true if this collider is overlapping the other collider. If
     *     they are overlapping, the minimum amount needed to move the shapes so
     *     that they are not overlapping is stored in out.
     *
     */
    bool getOverlap(
            tmat::Vector3f &out,
            const std::shared_ptr<Collider> &other,
            const tmat::Vector3f initialAxis = tmat::Vector3f::right,
            const int maxIterations = 0,
            const float tolerance = 0.000001f) const;

    void onSceneChanged(std::shared_ptr<Scene> newScene) override;

    std::shared_ptr<Node> clone() override
    {
        return std::make_shared<Collider>(shape());
    }

private:
    const std::shared_ptr<Shape> shape_;
    Box box_;

    typedef bool (Collider::*NearestSimplexFunction)(
            std::vector<tmat::Vector3f> &s,
            tmat::Vector3f &d,
            const float tolerance) const;

    /* Nearest simplex lookup table. */
    static NearestSimplexFunction nearestSimplexFunctions[4];

    /**
     * \brief
     *     Takes the simplex s and transforms it to a new simplex on s closest
     *     to the origin
     *
     * \details
     *     Stores the direction toward the origin normal to the
     *     simplex in d.
     *
     * \return True if s contains the origin.
     */
    bool nearestSimplex(
            std::vector<tmat::Vector3f> &s,
            tmat::Vector3f &d,
            const float tolerance) const;

    /* Implementations for each vertex count. */
    bool nearestSimplex1(
            std::vector<tmat::Vector3f> &s,
            tmat::Vector3f &d,
            const float tolerance) const;
    bool nearestSimplex2(
            std::vector<tmat::Vector3f> &s,
            tmat::Vector3f &d,
            const float tolerance) const;
    bool nearestSimplex3(
            std::vector<tmat::Vector3f> &s,
            tmat::Vector3f &d,
            const float tolerance) const;
    bool nearestSimplex4(
            std::vector<tmat::Vector3f> &s,
            tmat::Vector3f &d,
            const float tolerance) const;

    /**
     * \brief
     *     Finds the nearest triangle to the origin on the simplex s
     *
     * \details
     *     Finds the nearest triangle to the origin on the simplex s, calculates
     *     the direction from the nearest point to the origin to the origin
     *     (i.e. the nearest point negated) and stores it in d. The function
     *     returns the index in the index array, at which the three indices for
     *     the triangle start.
     *
     * \return The index of the triangle
     */
    int nearestTriangle(
            const std::vector<tmat::Vector3f> &s,
            const std::vector<int> &indices,
            tmat::Vector3f &d) const;

    friend class Scene;
};

} /* namespace */

#endif /* ifndef COLLIDER_HPP */
