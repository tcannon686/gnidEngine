#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <array>
#include <memory>
#include <functional>

#include "gnid/matrix/matrix.hpp"

namespace gnid
{

class Collider;

/**
 * \brief A collision between two colliders
 */
class Collision
{
public:
    /**
     * \brief
     *     Create a collision object given two colliders and the overlap between
     *     them, relative to the first collider
     */
    Collision(
            const std::shared_ptr<Collider> a,
            const std::shared_ptr<Collider> b,
            tmat::Vector3f overlap);

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

    /**
     * \brief
     *     Swap colliders positions and calculate the overlap relative to the
     *     first collider
     */
    void swapColliders();

    /**
     * \brief Return the collision with swapColliders() called
     */
    Collision swapped() const;

    bool operator==(const Collision &other) const;

private:
    std::array<std::shared_ptr<Collider>, 2> colliders_;
    mutable tmat::Vector3f overlap_;

    /* Whether or not the collider has been visited this cycle. */
    mutable bool visited_;

    friend class Scene;
};

} /* namespace */

namespace std
{
    template<>
    struct hash<gnid::Collision>
    {
        size_t operator()(const gnid::Collision &collision) const noexcept
        {
            auto h0 = std::hash<std::shared_ptr<gnid::Collider>>{}(
                    collision.colliders()[0]);
            auto h1 = std::hash<std::shared_ptr<gnid::Collider>>{}(
                    collision.colliders()[1]);
            
            return h0 ^ (h1 << 1);
        }
    };
}

#endif
