#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <memory>
#include <array>

#include "node.hpp"
#include "spatialnode.hpp"

namespace gnid
{

/**
 * \brief A rigid body
 */
class Rigidbody : public SpatialNode
{
public:
    /**
     * \brief Construct a rigid body
     */
    Rigidbody();

    void onSceneChanged(std::shared_ptr<Scene> newScene) override;
};

} /* namespace */

#endif /* ifndef RIGIDBODY_HPP */
