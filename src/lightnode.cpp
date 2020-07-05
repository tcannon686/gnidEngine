#include "renderer.hpp"
#include "scene.hpp"
#include "lightnode.hpp"

using namespace gnid;

void LightNode::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->renderer.remove(
                static_pointer_cast<LightNode>(shared_from_this()));
    if(newScene)
        newScene->renderer.add(
                static_pointer_cast<LightNode>(shared_from_this()));
}

