#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "gnid/node.hpp"

namespace gnid
{

/**
 * \brief A camera in the scene
 */
class Camera : public Node
{
    public:
        /**
         * \brief
         *     Constructs the camera's projection matrix from the given values
         */
        Camera(float fovy, float aspect, float znear, float zfar);

        /**
         * \brief Returns the camera projection matrix
         */
        const tmat::Matrix4f &projectionMatrix() const;
        
        /**
         * \brief
         *     Returns the camera view matrix, i.e. the inverse of the camera
         *     world matrix
         */
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

