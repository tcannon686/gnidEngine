#ifndef PHONGSHADER_HPP
#define PHONGSHADER_HPP

#include "shader.hpp"
#include "matrix.hpp"
#include "material.hpp"
using namespace tmat;

class PhongShader : public ShaderProgram
{
    private:
        GLint program;
    public:
        void init();
        void use();
        void setCameraMatrix(Matrix4f projection);
        void setTransformMatrix(int instance, Matrix4f transform);
        int getMaxInstances();
};

class PhongMaterial : public Material
{
    private:
        shared_ptr<PhongShader> shader;
    public:
        PhongMaterial(shared_ptr<PhongShader> shader) : shader(shader)
        {
        }

        shared_ptr<ShaderProgram> getShader()
        {
            return shader;
        }

        void bind()
        {
        }
};

#endif

