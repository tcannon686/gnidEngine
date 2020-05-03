#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <set>

#include "material.hpp"
#include "shader.hpp"
#include "node.hpp"
#include "renderer.hpp"
#include "camera.hpp"

using namespace std;

Binding::Binding(
    shared_ptr<Material> material,
    shared_ptr<RendererMesh> mesh,
    shared_ptr<Node> node) : material(material), mesh(mesh), node(node)
{
}

bool Binding::operator<(const Binding &other) const
{
    if(material->getShader() < other.material->getShader())
        return true;
    else if(material < other.material)
        return true;
    else if(mesh < other.mesh)
        return true;
    else if(node < other.node)
        return true;
    else
        return false;
}

void Renderer::renderMesh(
    shared_ptr<RendererMesh> mesh,
    int instanceCount) const
{
    glDrawElements(
        mesh->mode,
        mesh->count,
        mesh->type,
        nullptr);
}

void Renderer::render(shared_ptr<Camera> camera) const
{
    /* The material and mesh currently in use. */
    shared_ptr<Material> material = nullptr;
    shared_ptr<RendererMesh> mesh = nullptr;
    int instanceCount = 0;

    for(auto it = begin(bindings);
        it != end(bindings);
        ++ it)
    {
        if(it->material != material
                || it->material->getShader() != material->getShader())
        {
            if(instanceCount)
                renderMesh(mesh, instanceCount);
            if(!material || it->material->getShader() != material->getShader())
            {
                // Use shader
                shared_ptr<ShaderProgram> shader = it->material->getShader();
                shader->use();
                shader->setCameraMatrix(camera->getCameraMatrix());
            }
            material = it->material;
            material->bind();
            mesh = it->mesh;
            glBindVertexArray(mesh->vao);
            instanceCount = 1;
            it->material->getShader()->setTransformMatrix(
                    instanceCount,
                    it->node->getWorldMatrix());
        }
        else if(it->mesh != mesh)
        {
            if(instanceCount)
                renderMesh(mesh, instanceCount);
            mesh = it->mesh;
            glBindVertexArray(mesh->vao);
            instanceCount = 1;
            it->material->getShader()->setTransformMatrix(
                    instanceCount,
                    it->node->getWorldMatrix());
        }
        else
        {
            it->material->getShader()->setTransformMatrix(
                    instanceCount,
                    it->node->getWorldMatrix());
            instanceCount ++;
        }
    }
    if(instanceCount)
    {
        renderMesh(mesh, instanceCount);
    }
}

void Renderer::add(Binding binding)
{
    bindings.insert(binding);
}

void Renderer::remove(Binding binding)
{
    bindings.erase(bindings.find(binding));
}

