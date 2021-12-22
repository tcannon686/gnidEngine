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
        const std::shared_ptr<ShaderProgram> shader() const override
        {
            return shader_;
        }

        void bind() override;

        const std::shared_ptr<Texture2D> &diffuseTexture() const {
            return diffuseTexture_;
        }
        const tmat::Vector3f &diffuseColor() const { return diffuseColor_; }
        float diffuseMix() { return diffuseMix_; }
        const tmat::Vector3f &specularColor() const { return specularColor_; }
        float &specularExponent() { return specularExponent_; }

        /**
         * Builder class used to create PhongMaterial instances.
         */
        class Builder {
        public:
            Builder();
            Builder &shader(std::shared_ptr<PhongShader> shader);
            Builder &diffuse(float r, float g, float b);
            Builder &diffuse(const tmat::Vector3f &diffuse);
            Builder &diffuse(std::shared_ptr<Texture2D> diffuse);
            Builder &mix(float diffuseTextureMix);
            Builder &specular(const tmat::Vector3f &specular, float exponent);
            Builder &specular(float r, float g, float b);
            Builder &shininess(float exponent);
            std::shared_ptr<PhongMaterial> build();
        private:
            std::shared_ptr<Texture2D> diffuseTexture_;
            std::shared_ptr<PhongShader> shader_;
            tmat::Vector3f diffuseColor_;
            tmat::Vector3f specularColor_;
            float specularExponent_ = 0;
            float diffuseMix_ = 0;
        };

    private:
        std::shared_ptr<Texture2D> diffuseTexture_;
        std::shared_ptr<PhongShader> shader_;
        tmat::Vector3f diffuseColor_;
        tmat::Vector3f specularColor_;
        float specularExponent_;
        float diffuseMix_;

        PhongMaterial(
                std::shared_ptr<PhongShader> shader,
                tmat::Vector3f diffuseColor,
                std::shared_ptr<Texture2D> diffuseTexture,
                float diffuseMix,
                tmat::Vector3f specularColor,
                float specularExponent)
            : shader_(shader),
              diffuseColor_(diffuseColor),
              diffuseTexture_(diffuseTexture),
              diffuseMix_(diffuseMix),
              specularColor_(specularColor),
              specularExponent_(specularExponent)
        {
        }

};

} /* namespace */
#endif
