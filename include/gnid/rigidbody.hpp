#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <memory>
#include <array>

#include "gnid/node.hpp"
#include "gnid/spatialnode.hpp"

namespace gnid
{

/**
 * \brief A rigid body
 *
 * \details
 *     This node represents a rigid bodies. Rigid bodies should have at least
 *     once #Collider node as a descendant.
 *
 *     \note
 *         At the moment, the rigid body physics implemented here should not
 *         even really be called rigid body physics, as the physics here is
 *         **very** rudimentary. It gets the job done for now, don't judge me.
 * 
 * \todo
 *      - Add better physics. The physics right now is very rudimentary.
 */
class Rigidbody : public SpatialNode
{
public:
    /**
     * \brief Construct a rigid body
     */
    Rigidbody(float mass = 1.0f);

    /**
     * \brief The mass of the rigid body
     */
    float &mass();

    /**
     * \brief Apply an impulse to the rigid body
     */
    void addImpulse(const tmat::Vector3f &impulse);

    void onSceneChanged(std::shared_ptr<Scene> newScene) override;

    std::shared_ptr<Node> clone() override
    {
        return std::make_shared<Rigidbody>(*this);
    }

private:
    tmat::Vector3f velocity_;
    float mass_;

    friend class Scene;

    /**
     * \brief Update the rigid body's physics
     */
    void physicsUpdate(float dt);
};

} /* namespace */

#endif /* ifndef RIGIDBODY_HPP */
