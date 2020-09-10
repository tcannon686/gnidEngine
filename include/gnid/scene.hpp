#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <unordered_set>
#include "gnid/matrix/matrix.hpp"
#include "gnid/renderer.hpp"
#include "gnid/kdtree.hpp"
#include "gnid/collision.hpp"

namespace gnid
{

class Camera;
class Node;
class EmptyNode;
class Box;
class Rigidbody;
class RendererNode;
class Collider;

/**
 * \brief A scene
 *
 * \todo
 *     - Add more complex physics. The physics at the moment is very _very_
 *       rudimentary.
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
         * \brief The acceleration due to gravity in the scene
         */
        tmat::Vector3f &gravity();

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
        void handleCollision(
                std::shared_ptr<Collider> a,
                std::shared_ptr<Collider> b,
                tmat::Vector3f overlap);

        /**
         * \brief Update the momentums when two bodies collide
         */
        void updateMomentums(
                std::shared_ptr<Rigidbody> as,
                std::shared_ptr<Rigidbody> bs,
                const tmat::Vector3f &overlap);

        friend class Node;

        std::unordered_set<Collision> collisions;
        std::list<std::shared_ptr<Collider>> colliders;
        std::list<std::shared_ptr<Camera>> cameras;
        std::list<std::shared_ptr<Rigidbody>> rigidbodies;
        std::shared_ptr<KdTree> kdTree;
        KdTreePruner pruner;
        Renderer renderer;
        tmat::Vector3f gravity_;
};

}; /* namespace */

#endif

