#ifndef PHONGSHADER_HPP
#define PHONGSHADER_HPP

#include "gnid/glad/glad.h"
#include <GLFW/glfw3.h>

#include "gnid/texture.hpp"
#include "gnid/shader.hpp"
#include "gnid/matrix/matrix.hpp"
#include "gnid/material.hpp"

#include <array>

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
                std::shared_ptr<DirectionalLight> light) override;
        void setLight(
                int index,
                std::shared_ptr<Camera> camera,
                std::shared_ptr<PointLight> light) override;
        void setLight(
                std::shared_ptr<Camera> camera,
                std::shared_ptr<AmbientLight> light) override;
        void setDiffuseMix(float mix);
        void setDiffuseColor(tmat::Vector3f &diffuseColor);
        void setDiffuseTexture(std::shared_ptr<Texture2D> texture);
        void setSpecularColor(tmat::Vector3f &specularColor);
        void setSpecularExponent(float exponent);
        int getMaxInstances() override;

    private:
        GLint program = 0;
        GLint modelViewMatrixLoc = -1;
        GLint projectionMatrixLoc = -1;
        GLint lightCountLoc = -1;
        std::array<GLint, 32> lightsLocs;
        std::array<GLint, 32> lightColorsLocs;
        GLint diffuseColorLoc = -1;
        GLint diffuseMixLoc = -1;
        GLint diffuseTextureLoc = -1;
        GLint specularColorLoc = -1;
        GLint ambientColorLoc = -1;
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

        PhongMaterial()
        {
        }

        const std::shared_ptr<ShaderProgram> shader() const override
        {
            return shader_;
        }

        void bind() override;

        std::shared_ptr<Texture2D> &diffuseTexture() { return diffuseTexture_; }
        tmat::Vector3f &diffuseColor() { return diffuseColor_; }
        float &diffuseMix() { return diffuseMix_; }
        tmat::Vector3f &specularColor() { return specularColor_; }
        float &specularExponent() { return specularExponent_; }

    private:
        std::shared_ptr<Texture2D> diffuseTexture_;
        std::shared_ptr<PhongShader> shader_;
        tmat::Vector3f diffuseColor_;
        tmat::Vector3f specularColor_;
        float specularExponent_;
        float diffuseMix_;
};

} /* namespace */
#endif
