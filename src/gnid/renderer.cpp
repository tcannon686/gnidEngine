#include "gnid/renderer.hpp"

#include "gnid/glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <set>
#include <cassert>

#include "gnid/material.hpp"
#include "gnid/shader.hpp"
#include "gnid/node.hpp"
#include "gnid/camera.hpp"
#include "gnid/lightnode.hpp"
#include "gnid/pointlight.hpp"
#include "gnid/directionallight.hpp"

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
    if(material->shader() < other.material->shader())
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
        /* Set the light depending on its type. */
        auto dl = (*it)->as<DirectionalLight>();
        auto pl = (*it)->as<PointLight>();

        if(dl)
        {
            program->setLight(i, camera, dl);
            ++ i;
        }
        else if(pl)
        {
            program->setLight(i, camera, pl);
        }
        else
        {
            /* TODO throw exception. */
            assert(false);
        }
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
                || it->material->shader() != material->shader())
        {
            if(instanceCount)
                renderMesh(mesh, instanceCount);
            if(!material || it->material->shader() != material->shader())
            {
                /* Use shader. */
                shared_ptr<ShaderProgram> shader = it->material->shader();
                shader->use();
                shader->setProjectionMatrix(camera->projectionMatrix());
            }
            material = it->material;
            material->bind();
            mesh = it->mesh;
            glBindVertexArray(mesh->vao);
            instanceCount = 1;
            it->material->shader()->setModelViewMatrix(
                    instanceCount,
                    camera->viewMatrix() * it->node->worldMatrix());
            updateLights(camera, it->material->shader());
        }
        else if(it->mesh != mesh)
        {
            if(instanceCount)
                renderMesh(mesh, instanceCount);
            mesh = it->mesh;
            glBindVertexArray(mesh->vao);
            instanceCount = 1;
            it->material->shader()->setModelViewMatrix(
                    instanceCount,
                    camera->projectionMatrix() * it->node->worldMatrix());
            updateLights(camera, it->material->shader());
        }
        else
        {
            /* If we've reached the max number of instances, draw them. */
            if(instanceCount >= it->material->shader()->getMaxInstances())
            {
                renderMesh(mesh, instanceCount);
                instanceCount = 0;
            }

            it->material->shader()->setModelViewMatrix(
                    instanceCount,
                    camera->viewMatrix() * it->node->worldMatrix());
            updateLights(camera, it->material->shader());
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


