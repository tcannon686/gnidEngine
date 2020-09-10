#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <memory>
#include <functional>

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
          overlap_((a < b ? overlap : -overlap)),
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
