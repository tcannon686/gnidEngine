#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "node.hpp"

namespace gnid
{

class Camera : public Node
{
    private:
        Matrix4f projectionMatrix;
    public:
        Camera(float fovy, float aspect, float znear, float zfar);
        Matrix4f getProjectionMatrix();
        Matrix4f getViewMatrix();
};

}; /* namespace */

#endif

