#include "gnid/rigidbody.hpp"

#include "gnid/scene.hpp"

using namespace std;
using namespace tmat;
using namespace gnid;

Rigidbody::Rigidbody(float mass)
    : mass_(mass)
{
}

void Rigidbody::physicsUpdate(float dt)
{
    transformWorld(getTranslateMatrix(velocity_ * dt));
}

void Rigidbody::addImpulse(const Vector3f &impulse)
{
    velocity_ += impulse * (1.0f / mass_);
}

float &Rigidbody::mass()
{
    return mass_;
}

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


