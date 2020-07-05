#ifndef SHADER_HPP
#define SHADER_HPP

#include "matrix/matrix.hpp"

namespace gnid
{

using namespace tmat;

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
        virtual void setLightPosition(int index, Vector3f position) = 0;
        virtual int getMaxInstances() = 0;
};

}; /* namespace */
#endif
