#include "gnid/lightnode.hpp"

#include <memory>

#include "gnid/renderer.hpp"
#include "gnid/scene.hpp"

using namespace std;
using namespace gnid;

void LightNode::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->registerNode(
                static_pointer_cast<LightNode>(shared_from_this()));
    if(newScene)
        newScene->registerNode(
                static_pointer_cast<LightNode>(shared_from_this()));
}

std::shared_ptr<Node> LightNode::clone()
{
    auto ret = std::make_shared<LightNode>(*this);
    ret->cloneChildren(shared_from_this());
    return ret;
}

