#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "gnid/node.hpp"

namespace gnid
{

class Camera : public Node
{
    private:
        tmat::Matrix4f projectionMatrix;
    public:
        Camera(float fovy, float aspect, float znear, float zfar);
        tmat::Matrix4f getProjectionMatrix();
        tmat::Matrix4f getViewMatrix();
        void onSceneChanged(std::shared_ptr<Scene> newScene) override;
        
        std::shared_ptr<Node> clone() override
        {
            return std::make_shared<Camera>(*this);
        }
};

}; /* namespace */

#endif

