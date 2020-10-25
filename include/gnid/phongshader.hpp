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
 * \brief A simple shader using the Blinn-Phong reflection model
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
        void setSpecularColor(tmat::Vector3f &specularColor);
        void setSpecularExponent(float exponent);
        int getMaxInstances() override;

    private:
        GLint program = 0;
        GLint modelViewMatrixLoc = -1;
        GLint projectionMatrixLoc = -1;
        GLint lightCountLoc = -1;
        std::array<GLint, 32> lightsLocs;
        GLint diffuseColorLoc = -1;
        GLint specularColorLoc = -1;
        GLint specularExponentLoc = -1;
};

/**
 * \brief A material for the phong shader
 */
class PhongMaterial : public Material
{
    public:
        PhongMaterial(
                std::shared_ptr<PhongShader> shader,
                tmat::Vector3f diffuseColor,
                tmat::Vector3f specularColor,
                float specularExponent)
            : shader_(shader),
              diffuseColor_(diffuseColor),
              specularColor_(specularColor),
              specularExponent_(specularExponent)
        {
        }

        const std::shared_ptr<ShaderProgram> shader() const override
        {
            return shader_;
        }

        void bind() override;

        tmat::Vector3f &diffuseColor();
        tmat::Vector3f &specularColor();
        float &specularExponent();

    private:
        std::shared_ptr<PhongShader> shader_;
        tmat::Vector3f diffuseColor_;
        tmat::Vector3f specularColor_;
        float specularExponent_;
};

} /* namespace */
#endif
