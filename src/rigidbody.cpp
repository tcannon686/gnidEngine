#include "rigidbody.hpp"
#include "scene.hpp"

using namespace gnid;

void Rigidbody::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->unregisterNode(
                static_pointer_cast<Rigidbody>(shared_from_this()));
    if(newScene)
        newScene->registerNode(
                static_pointer_cast<Rigidbody>(shared_from_this()));
}


