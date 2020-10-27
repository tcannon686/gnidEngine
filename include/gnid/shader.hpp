#ifndef SHADER_HPP
#define SHADER_HPP

#include <memory>

#include "gnid/matrix/matrix.hpp"

namespace gnid
{

class DirectionalLight;
class PointLight;
class LightNode;
class Camera;

/**
 * \brief An abstract shader program
 */
class ShaderProgram
{
    public:
        static const int ATTRIB_LOCATION_VERTEX = 1;
        static const int ATTRIB_LOCATION_NORMAL = 2;
        static const int ATTRIB_LOCATION_TEXCO = 3;

        /**
         * \brief Initialize the shader object on the GPU
         */
        virtual void init() = 0;

        /**
         * \brief Use this shader as the current shader program
         */
        virtual void use() = 0;

        /**
         * \brief Set the current projection matrix for the shader
         */
        virtual void setProjectionMatrix(tmat::Matrix4f projection) = 0;

        /**
         * \brief
         *     Set the transformation of the scene in the shader for the given
         *     mesh instance
         */
        virtual void setModelViewMatrix(
                int instance,
                tmat::Matrix4f transform) = 0;

        /**
         * \brief Set the number of lights to be rendered
         */
        virtual void setLightCount(int count) = 0;

        /**
         * \brief Create a light in the scene from a point light node
         */
        virtual void setLight(
                int index,
                std::shared_ptr<Camera> camera,
                std::shared_ptr<PointLight> light) = 0;

        /**
         * \brief Create a light in the scene from a directional light node
         */
        virtual void setLight(
                int index,
                std::shared_ptr<Camera> camera,
                std::shared_ptr<DirectionalLight> light) = 0;

        /**
         * \brief
         *     Returns the maximum number of mesh instances supported by the
         *     shader
         */
        virtual int getMaxInstances() = 0;
};

}; /* namespace */
#endif
