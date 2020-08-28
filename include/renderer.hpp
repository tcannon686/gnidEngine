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
using namespace std;

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
        const shared_ptr<Material> material;
        const shared_ptr<RendererMesh> mesh;
        const shared_ptr<Node> node;

        Binding(
            shared_ptr<Material> material,
            shared_ptr<RendererMesh> mesh,
            shared_ptr<Node> node);

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
        void render(shared_ptr<Camera> camera) const;

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
        void add(shared_ptr<LightNode> light);

        /**
         * \brief Remove a light from the rendered scene
         */
        void remove(shared_ptr<LightNode> light);
    private:
        set<Binding> bindings;
        set<shared_ptr<LightNode>> lights;
        void renderMesh(
            shared_ptr<RendererMesh> mesh,
            int instanceCount) const;
        void updateLights(
                shared_ptr<Camera> camera,
                shared_ptr<ShaderProgram> program) const;
};

}; /* namespace */
#endif
