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

