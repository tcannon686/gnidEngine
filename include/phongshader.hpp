#ifndef PHONGSHADER_HPP
#define PHONGSHADER_HPP

#include "shader.hpp"
#include "matrix/matrix.hpp"
#include "material.hpp"

using namespace gnid;
using namespace tmat;

class PhongShader : public ShaderProgram
{
    private:
        GLint program;
    public:
        void init() override;
        void use() override;
        void setProjectionMatrix(Matrix4f projection) override;
        void setModelViewMatrix(int instance, Matrix4f transform) override;
        int getMaxInstances() override;
};

class PhongMaterial : public Material
{
    private:
        shared_ptr<PhongShader> shader;
    public:
        PhongMaterial(shared_ptr<PhongShader> shader) : shader(shader)
        {
        }

        shared_ptr<ShaderProgram> getShader() override
        {
            return shader;
        }

        void bind() override
        {
        }
};

#endif

