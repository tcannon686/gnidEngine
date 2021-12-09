#include "gnid/directionallight.hpp"

using namespace std;
using namespace gnid;
using namespace tmat;

DirectionalLight::DirectionalLight()
    : direction_(Vector3f::up)
{
}

tmat::Vector3f &DirectionalLight::direction()
{
    return direction_;
}

shared_ptr<Node> DirectionalLight::clone()
{
    auto ret = std::make_shared<DirectionalLight>(*this);
    ret->cloneChildren(shared_from_this());
    return ret;
}

void DirectionalLight::setLight(
    int index,
    shared_ptr<Camera> camera,
    shared_ptr<ShaderProgram> program)
{
    program->setLight(
        index,
        camera,
        static_pointer_cast<DirectionalLight>(shared_from_this()));
}
