#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>

namespace gnid
{

/* Forward declarations. */
class ShaderProgram;

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
        virtual const std::shared_ptr<ShaderProgram> shader() const = 0;

        /**
         * \brief
         *     Attaches the shader to the current context and uploads the
         *     material's properties to the shader
         */
        virtual void bind() = 0;
};

}; /* namespace */

#endif
