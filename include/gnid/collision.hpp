#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <memory>

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

    bool operator<(const Collision &other) const
    {
        for(int i = 0; i < 2; i ++)
        {
            if(colliders_[i] < other.colliders_[i])
                return true;
        }
        return false;
    }

private:
    std::array<std::shared_ptr<Collider>, 2> colliders_;
    mutable tmat::Vector3f overlap_;

    /* Whether or not the collider has been visited this cycle. */
    mutable bool visited_;

    friend class Scene;
};

} /* namespace */

#endif
