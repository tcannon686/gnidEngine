#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "gnid/node.hpp"

namespace gnid
{

class Camera : public Node
{
    public:
        Camera(float fovy, float aspect, float znear, float zfar);
        const tmat::Matrix4f &projectionMatrix() const;
        const tmat::Matrix4f &viewMatrix() const;
        void onSceneChanged(std::shared_ptr<Scene> newScene) override;
        
        std::shared_ptr<Node> clone() override
        {
            auto ret = std::make_shared<Camera>(*this);
            ret->cloneChildren(shared_from_this());
            return ret;
        }
    private:
        tmat::Matrix4f projectionMatrix_;
};

}; /* namespace */

#endif

