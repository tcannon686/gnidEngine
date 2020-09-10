#include "gnid/camera.hpp"

#include <memory>
#include <cmath>

#include "gnid/matrix/matrix.hpp"
#include "gnid/scene.hpp"

using namespace tmat;
using namespace gnid;
using namespace std;

Camera::Camera(float fovy, float aspect, float znear, float zfar)
{
    float f = 1 / tan(fovy / 2);
    float m11 = f / aspect;
    float m22 = f;
    float m33 = (znear + zfar) / (znear - zfar);
    float m34 = 2 * zfar * znear / (znear - zfar);
    
    projectionMatrix_ = Matrix4f {
        { m11,   0,   0,   0 },
        {   0, m22,   0,   0 },
        {   0,   0, m33, m34 },
        {   0,   0,  -1,   0 }
    };
}

const Matrix4f &Camera::projectionMatrix() const
{
    return projectionMatrix_;
}

const Matrix4f &Camera::viewMatrix() const
{
    return worldMatrixInverse();
}

void Camera::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->unregisterNode(
                static_pointer_cast<Camera>(shared_from_this()));
    if(newScene)
        newScene->registerNode(
                static_pointer_cast<Camera>(shared_from_this()));
}

