#ifndef PHONGSHADER_HPP
#define PHONGSHADER_HPP

#include "gnid/glad/glad.h"
#include <GLFW/glfw3.h>

#include "gnid/shader.hpp"
#include "gnid/matrix/matrix.hpp"
#include "gnid/material.hpp"

namespace gnid
{

/**
 * \brief A simple shader using the phong reflection model
 */
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
        void setDiffuseColor(tmat::Vector3f &diffuseColor);
        int getMaxInstances() override;

    private:
        GLint program;
        GLint modelViewMatrixLoc;
        GLint projectionMatrixLoc;
        GLint lightCountLoc;
        std::array<GLint, 32> lightsLocs;
        GLint diffuseColorLoc;
};

/**
 * \brief A material for the phong shader
 */
class PhongMaterial : public Material
{
    public:
        PhongMaterial(
                std::shared_ptr<PhongShader> shader,
                tmat::Vector3f diffuseColor)
            : shader_(shader),
              diffuseColor_(diffuseColor)
        {
        }

        const std::shared_ptr<ShaderProgram> shader() const override
        {
            return shader_;
        }

        void bind() override;

        tmat::Vector3f &diffuseColor();

    private:
        std::shared_ptr<PhongShader> shader_;
        tmat::Vector3f diffuseColor_;
};

} /* namespace */
#endif
