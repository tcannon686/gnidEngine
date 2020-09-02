#ifndef RENDERERNODE_HPP
#define RENDERERNODE_HPP

#include "gnid/renderer.hpp"
#include "gnid/node.hpp"

namespace gnid
{

/* Forward declarations. */
class Scene;
class Material;

/**
 * \brief A node capable of rendering a mesh
 */
class RendererNode : public Node
{
    public:
        /**
         * \brief The mesh to be rendered
         */
        const std::shared_ptr<RendererMesh> mesh;

        /**
         * \brief The material to render the mesh with
         */
        const std::shared_ptr<Material> material;
        RendererNode(
            std::shared_ptr<RendererMesh> mesh,
            std::shared_ptr<Material> material);
        void onSceneChanged(std::shared_ptr<Scene> newScene);
        std::shared_ptr<Node> clone();
};

} /* namespace */

#endif
