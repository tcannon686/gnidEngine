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
        void onSceneChanged(shared_ptr<Scene> newScene) override;
        
        shared_ptr<Node> clone() override
        {
            return make_shared<Camera>(*this);
        }
};

}; /* namespace */

#endif

