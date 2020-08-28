#include <memory>

#include "renderer.hpp"
#include "scene.hpp"
#include "lightnode.hpp"

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

shared_ptr<Node> LightNode::clone()
{
    return make_shared<LightNode>(*this);
}

