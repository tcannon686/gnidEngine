#ifndef PHONGSHADER_HPP
#define PHONGSHADER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "matrix/matrix.hpp"
#include "material.hpp"

namespace gnid
{

class PhongShader : public ShaderProgram
{
    public:
        void init() override;
        void use() override;
        void setProjectionMatrix(tmat::Matrix4f projection) override;
        void setModelViewMatrix(
                int instance,
                tmat::Matrix4f transform) override;
        void setLightCount(int count) override;
        void setLight(
                int index,
                std::shared_ptr<Camera> camera,
                std::shared_ptr<LightNode> light) override;
        int getMaxInstances() override;
    private:
        GLint program;
        GLint modelViewMatrixLoc;
        GLint projectionMatrixLoc;
        GLint lightCountLoc;
        std::array<GLint, 32> lightsLocs;
};

class PhongMaterial : public Material
{
    private:
        std::shared_ptr<PhongShader> shader;
    public:
        PhongMaterial(std::shared_ptr<PhongShader> shader) : shader(shader)
        {
        }

        std::shared_ptr<ShaderProgram> getShader() override
        {
            return shader;
        }

        void bind() override
        {
        }
};

} /* namespace */
#endif
