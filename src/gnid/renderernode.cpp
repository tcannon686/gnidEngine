#include "gnid/renderernode.hpp"

#include "gnid/scene.hpp"
#include "gnid/renderer.hpp"
#include "gnid/material.hpp"

using namespace std;
using namespace gnid;

RendererNode::RendererNode(
        shared_ptr<RendererMesh> mesh,
        shared_ptr<Material> material)
    : mesh(mesh), material(material)
{
}

void RendererNode::onSceneChanged(shared_ptr<Scene> newScene)
{
    auto oldScene = getScene().lock();

    if(oldScene)
        oldScene->unregisterNode(
                static_pointer_cast<RendererNode>(shared_from_this()));
    if(newScene)
        newScene->registerNode(
                static_pointer_cast<RendererNode>(shared_from_this()));
}

std::shared_ptr<Node> RendererNode::clone()
{
    auto ret = std::make_shared<RendererNode>(*this);
    ret->cloneChildren(shared_from_this());
    return ret;
}

