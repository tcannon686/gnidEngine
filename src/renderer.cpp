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
#include "lightnode.hpp"

using namespace gnid;
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
    glDrawElementsInstanced(
        mesh->mode,
        mesh->count,
        mesh->type,
        nullptr,
        instanceCount);
}

void Renderer::updateLights(
    shared_ptr<Camera> camera,
    shared_ptr<ShaderProgram> program) const
{
    program->setLightCount(lights.size());
    int i = 0;
    for(auto it = begin(lights);
            it != end(lights);
            ++ it)
    {
        program->setLight(i, camera, *it);
        ++ i;
    }
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
                /* Use shader. */
                shared_ptr<ShaderProgram> shader = it->material->getShader();
                shader->use();
                shader->setProjectionMatrix(camera->getProjectionMatrix());
            }
            material = it->material;
            material->bind();
            mesh = it->mesh;
            glBindVertexArray(mesh->vao);
            instanceCount = 1;
            it->material->getShader()->setModelViewMatrix(
                    instanceCount,
                    camera->getViewMatrix() * it->node->getWorldMatrix());
            updateLights(camera, it->material->getShader());
        }
        else if(it->mesh != mesh)
        {
            if(instanceCount)
                renderMesh(mesh, instanceCount);
            mesh = it->mesh;
            glBindVertexArray(mesh->vao);
            instanceCount = 1;
            it->material->getShader()->setModelViewMatrix(
                    instanceCount,
                    camera->getViewMatrix() * it->node->getWorldMatrix());
            updateLights(camera, it->material->getShader());
        }
        else
        {
            /* If we've reached the max number of instances, draw them. */
            if(instanceCount >= it->material->getShader()->getMaxInstances())
            {
                renderMesh(mesh, instanceCount);
                instanceCount = 0;
            }

            it->material->getShader()->setModelViewMatrix(
                    instanceCount,
                    camera->getViewMatrix() * it->node->getWorldMatrix());
            updateLights(camera, it->material->getShader());
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

void Renderer::add(shared_ptr<LightNode> light)
{
    lights.insert(light);
}

void Renderer::remove(shared_ptr<LightNode> light)
{
    lights.erase(lights.find(light));
}


