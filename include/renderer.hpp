#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <memory>
#include <set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "node.hpp"

namespace gnid
{

/* Forward declarations. */
class ShaderProgram;
class LightNode;

/**
 * \brief A mesh for rendering with OpenGL
 */
class RendererMesh
{
    public:
        const GLenum mode;
        const GLsizei count;
        const GLenum type;
        const GLuint vao;

        RendererMesh(GLenum mode, GLsizei count, GLenum type, GLuint vao)
            : mode(mode), count(count), type(type), vao(vao)
        {
        }
};

/**
 * \brief The paring of a material, a mesh, and a node
 *
 * \details
 *     Bindings are created to represent individual rendering units within the
 *     renderer. They are stored in a map and sorted by mesh, material, and
 *     node, so that information can be reused between render calls.
 */
class Binding
{
    public:
        const std::shared_ptr<Material> material;
        const std::shared_ptr<RendererMesh> mesh;
        const std::shared_ptr<Node> node;

        Binding(
            std::shared_ptr<Material> material,
            std::shared_ptr<RendererMesh> mesh,
            std::shared_ptr<Node> node);

        bool operator<(const Binding &other) const;
};

/**
 * \brief The renderer for the game
 *
 * \details
 *     Currently uses OpenGL
 */
class Renderer
{
    public:
        /**
         * \brief Render from the given camera
         */
        void render(std::shared_ptr<Camera> camera) const;

        /**
         * \brief Add a binding to be rendered
         */
        void add(Binding binding);

        /**
         * \brief Remove a binding
         */
        void remove(Binding binding);

        /**
         * \brief Add a light to the rendered scene
         */
        void add(std::shared_ptr<LightNode> light);

        /**
         * \brief Remove a light from the rendered scene
         */
        void remove(std::shared_ptr<LightNode> light);
    private:
        std::set<Binding> bindings;
        std::set<std::shared_ptr<LightNode>> lights;
        void renderMesh(
            std::shared_ptr<RendererMesh> mesh,
            int instanceCount) const;
        void updateLights(
                std::shared_ptr<Camera> camera,
                std::shared_ptr<ShaderProgram> program) const;
};

}; /* namespace */
#endif
