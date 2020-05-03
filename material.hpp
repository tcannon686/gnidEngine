#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>
#include "shader.hpp"

class Material
{
    private:
    public:
        virtual shared_ptr<ShaderProgram> getShader() = 0;
        virtual void bind() = 0;
};

#endif
