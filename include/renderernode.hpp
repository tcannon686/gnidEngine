#ifndef RENDERERNODE_HPP
#define RENDERERNODE_HPP

#include "renderer.hpp"
#include "node.hpp"

namespace gnid
{

/**
 * \brief A node capable of rendering a mesh
 */
class RendererNode : public Node
{
    public:
        /**
         * \brief The mesh to be rendered
         */
        const shared_ptr<RendererMesh> mesh;

        /**
         * \brief The material to render the mesh with
         */
        const shared_ptr<Material> material;
        RendererNode(
            shared_ptr<RendererMesh> mesh,
            shared_ptr<Material> material);
        void onSceneChanged(shared_ptr<Scene> newScene);
        shared_ptr<Node> clone();
};

} /* namespace */

#endif
