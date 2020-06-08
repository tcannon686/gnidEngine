#ifndef SHADER_HPP
#define SHADER_HPP

#include "matrix.hpp"
using namespace tmat;

class ShaderProgram
{
    public:
        static const int ATTRIB_LOCATION_VERTEX = 1;
        static const int ATTRIB_LOCATION_NORMAL = 2;
        static const int ATTRIB_LOCATION_TEXCO = 3;

        virtual void init() = 0;
        virtual void use() = 0;
        virtual void setCameraMatrix(Matrix4f projection) = 0;
        virtual void setTransformMatrix(int instance, Matrix4f transform) = 0;
        virtual int getMaxInstances() = 0;
};

#endif
