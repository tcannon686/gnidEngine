#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <memory>
#include <list>
#include <vector>
#include <queue>

#include "gnid/matrix/matrix.hpp"
#include "gnid/box.hpp"
#include "gnid/node.hpp"

namespace gnid
{

class Shape;
class Collider;

/**
 * \brief A collision between two colliders
 */
class Collision
{
public:
    /**
     * \brief The colliders involved in the collision
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
 * \brief A node capable of collision
 *
 * \details
 *     This type of node should have a Rigidbody node as an ancestor. If a it
 *     does not have a Rigidbody as an ancestor, the collider will be static,
 *     meaning it will not move during collisions but can still be collided
 *     against. If it does have a Rigidbody as an ancestor, its behavior is
 *     determined by its first Rigidbody ancestor.
 *
 * \todo
 *      - Add caching of initial axis for GJK.
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
            const float tolerance = 0.001f) const;

    bool &isTrigger();

    void onSceneChanged(std::shared_ptr<Scene> newScene) override;

    std::shared_ptr<Node> clone() override
    {
        return std::make_shared<Collider>(shape());
    }

private:

    /**
     * \brief A triangle class used in the EPA algorithm
     */
    class Triangle
    {
    public:
        Triangle(
                const int ia,
                const int ib,
                const int ic,
                const std::vector<tmat::Vector3f> &vertices);

        const int &indexA() const { return ia; }
        const int &indexB() const { return ib; }
        const int &indexC() const { return ic; }

        const tmat::Vector3f &normal() const { return normal_; }
        const float &distance() const { return distance_; };

        /**
         * \brief
         *     Returns true if this triangle is closer to the origin than the
         *     other
         */
        bool operator<(const Triangle &other) const;

    private:
        int ia;
        int ib;
        int ic;
        tmat::Vector3f normal_;
        float distance_;
    };

    const std::shared_ptr<Shape> shape_;
    Box box_;
    bool isTrigger_;

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

    bool gjk(
            tmat::Vector3f &d,
            std::vector<tmat::Vector3f> &s,
            const std::shared_ptr<Collider> &other,
            const float tolerance) const;

    void epa(
            tmat::Vector3f &out,
            std::priority_queue<Triangle> &triangles,
            std::vector<tmat::Vector3f> &vertices,
            const std::shared_ptr<Collider> &other,
            const float tolerance) const;

    friend class Scene;
};

} /* namespace */

#endif /* ifndef COLLIDER_HPP */
