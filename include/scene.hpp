#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <set>
#include "matrix/matrix.hpp"
#include "renderer.hpp"
#include "collider.hpp"

namespace gnid
{

class Camera;
class Node;
class EmptyNode;
class Box;
class Rigidbody;

using namespace tmat;

class Scene : public enable_shared_from_this<Scene>
{
    public:
        const shared_ptr<Node> root;
        Renderer renderer;

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
         * \brief Register a special node for use in the scene
         *
         * \details
         *     This function should be called by certain special node types to
         *     make them usable by the scene. For example, colliders should call
         *     this function to make themselves visible to the physics system.
         */
        void registerNode(shared_ptr<Collider> collider);
        void registerNode(shared_ptr<Camera> camera);
        void registerNode(shared_ptr<Rigidbody> rigidbody);

        /**
         * \brief Unregister a special node
         */
        void unregisterNode(shared_ptr<Collider> collider);
        void unregisterNode(shared_ptr<Camera> camera);
        void unregisterNode(shared_ptr<Rigidbody> rigidbody);
    private:
        friend class Node;

        set<Collision> collisions;
        list<shared_ptr<Collider>> colliders;
        list<shared_ptr<Camera>> cameras;
        list<shared_ptr<Rigidbody>> rigidbodies;
};

}; /* namespace */

#endif

