#ifndef RENDERERNODE_HPP
#define RENDERERNODE_HPP

#include "renderer.hpp"
#include "node.hpp"

namespace gnid
{

class RendererNode : public Node
{
    public:
        const shared_ptr<RendererMesh> mesh;
        const shared_ptr<Material> material;
        RendererNode(
            shared_ptr<RendererMesh> mesh,
            shared_ptr<Material> material);
        void onSceneChanged(shared_ptr<Scene> newScene);
        shared_ptr<Node> clone();
};

} /* namespace */

#endif
