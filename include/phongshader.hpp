#ifndef PHONGSHADER_HPP
#define PHONGSHADER_HPP

#include "shader.hpp"
#include "matrix/matrix.hpp"
#include "material.hpp"

using namespace gnid;
using namespace tmat;

class PhongShader : public ShaderProgram
{
    public:
        void init() override;
        void use() override;
        void setProjectionMatrix(Matrix4f projection) override;
        void setModelViewMatrix(int instance, Matrix4f transform) override;
        void setLightCount(int count) override;
        void setLight(
                int index,
                shared_ptr<Camera> camera,
                shared_ptr<LightNode> light) override;
        int getMaxInstances() override;
    private:
        GLint program;
        GLint modelViewMatrixLoc;
        GLint projectionMatrixLoc;
        GLint lightCountLoc;
        array<GLint, 32> lightsLocs;
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

