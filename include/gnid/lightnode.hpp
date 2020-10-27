#ifndef LIGHTNODE_HPP
#define LIGHTNODE_HPP

#include "gnid/node.hpp"

namespace gnid
{

/**
 * \brief A light in a scene
 */
class LightNode : public Node
{
public:
    void onSceneChanged(std::shared_ptr<Scene> newScene) override;
};

}; /* namespace */

#endif /* ifndef LIGHTNODE_HPP */

