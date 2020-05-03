#include "renderernode.hpp"
#include "scene.hpp"
#include "renderer.hpp"
#include "material.hpp"

RendererNode::RendererNode(
        shared_ptr<RendererMesh> mesh,
        shared_ptr<Material> material)
    : mesh(mesh), material(material)
{
}

void RendererNode::onSceneChanged(shared_ptr<Scene> newScene)
{
    Binding binding(material, mesh, shared_from_this());
    auto oldScene = scene.lock();

    if(oldScene)
        oldScene->renderer.remove(binding);
    if(newScene)
        newScene->renderer.add(binding);
}
