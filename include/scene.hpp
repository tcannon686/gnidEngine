#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <set>
#include "matrix/matrix.hpp"
#include "renderer.hpp"
#include "collider.hpp"
#include "kdtree.hpp"

namespace gnid
{

class Camera;
class Node;
class EmptyNode;
class Box;
class Rigidbody;
class RendererNode;

/**
 * \brief A scene
 */
class Scene : public std::enable_shared_from_this<Scene>
{
    public:
        const std::shared_ptr<Node> root;

        Scene();

        /**
         * \brief Initialize the scene
         *
         * \details
         *     This method should be called after the scene is created using the
         *     constructor.
         */
        void init();

        /**
         * \brief Render the scene
         */
        void render();

        /**
         * \brief Update the scene using a timestep
         *
         * \param dt The elapsed time in seconds
         */
        void update(float dt);

        /**
         * \brief
         *     Update the world matrices of all of the scenes nodes from their
         *     local matrices
         */
        void updateWorldMatrix();

        /**
         * \brief Register a collider node for use in the scene
         */
        void registerNode(std::shared_ptr<Collider> collider);

        /**
         * \brief Register a camera node for use in the scene
         */
        void registerNode(std::shared_ptr<Camera> camera);

        /**
         * \brief Register a rigidbody node for use in the scene
         */
        void registerNode(std::shared_ptr<Rigidbody> rigidbody);

        /**
         * \brief Register a render node for use in the scene
         */
        void registerNode(std::shared_ptr<RendererNode> renderNode);

        /**
         * \brief Register a light node for use in the scene
         */
        void registerNode(std::shared_ptr<LightNode> lightNode);

        /**
         * \brief Unregister a collider node
         */
        void unregisterNode(std::shared_ptr<Collider> collider);

        /**
         * \brief Unregister a camera node
         */
        void unregisterNode(std::shared_ptr<Camera> camera);

        /**
         * \brief Unregister a rigidbody node
         */
        void unregisterNode(std::shared_ptr<Rigidbody> rigidbody);

        /**
         * \brief Unregister a renderer node
         */
        void unregisterNode(std::shared_ptr<RendererNode> rendererNode);

        /**
         * \brief Unregister a light node
         */
        void unregisterNode(std::shared_ptr<LightNode> lightNode);
    private:
        friend class Node;

        std::set<Collision> collisions;
        std::list<std::shared_ptr<Collider>> colliders;
        std::list<std::shared_ptr<Camera>> cameras;
        std::list<std::shared_ptr<Rigidbody>> rigidbodies;
        std::shared_ptr<KdTree> kdTree;
        KdTreePruner pruner;
        Renderer renderer;
};

}; /* namespace */

#endif

