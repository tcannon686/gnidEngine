#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

#include <memory>
#include <array>

#include "node.hpp"

namespace gnid
{

using namespace tmat;
using namespace std;

/**
 * \brief A rigid body
 */
class Rigidbody : public Node
{
public:
    /**
     * \brief Construct a rigid body
     */
    Rigidbody();

    void onSceneChanged(shared_ptr<Scene> newScene) override;
};

} /* namespace */

#endif /* ifndef RIGIDBODY_HPP */
