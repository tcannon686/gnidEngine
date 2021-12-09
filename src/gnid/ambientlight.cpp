#include "gnid/ambientlight.hpp"

using namespace std;
using namespace tmat;
using namespace gnid;

AmbientLight::AmbientLight()
{
}

void AmbientLight::setLight(
    int index,
    shared_ptr<Camera> camera,
    shared_ptr<ShaderProgram> program)
{
    program->setLight(
        camera,
        static_pointer_cast<AmbientLight>(shared_from_this()));
}

shared_ptr<Node> AmbientLight::clone()
{
    auto ret = std::make_shared<AmbientLight>(*this);
    ret->cloneChildren(shared_from_this());
    return ret;
}
