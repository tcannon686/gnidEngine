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

class Renderer
{
    public:
        void render(shared_ptr<Camera> camera) const;
        void add(Binding binding);
        void remove(Binding binding);
        void add(shared_ptr<LightNode> light);
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
