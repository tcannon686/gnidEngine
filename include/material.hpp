#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include "shader.hpp"

namespace gnid
{

/**
 * \brief A generic material
 */
class Material
{
    private:
    public:
        /**
         * \brief Returns the shader used by the material
         */
        virtual shared_ptr<ShaderProgram> getShader() = 0;

        /**
         * \brief
         *     Attaches the shader to the current context and uploads the
         *     material's properties to the shader
         */
        virtual void bind() = 0;
};

}; /* namespace */

#endif
