#ifndef SHADER_HPP
#define SHADER_HPP

#include "matrix/matrix.hpp"

namespace gnid
{

using namespace tmat;

class LightNode;
class Camera;

class ShaderProgram
{
    public:
        static const int ATTRIB_LOCATION_VERTEX = 1;
        static const int ATTRIB_LOCATION_NORMAL = 2;
        static const int ATTRIB_LOCATION_TEXCO = 3;

        virtual void init() = 0;
        virtual void use() = 0;
        virtual void setProjectionMatrix(Matrix4f projection) = 0;
        virtual void setModelViewMatrix(int instance, Matrix4f transform) = 0;
        virtual void setLightCount(int count) = 0;
        virtual void setLight(
                int index,
                shared_ptr<Camera> camera,
                shared_ptr<LightNode> light) = 0;
        virtual int getMaxInstances() = 0;
};

}; /* namespace */
#endif
