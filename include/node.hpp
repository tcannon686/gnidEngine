#ifndef NODE_HPP
#define NODE_HPP

#include "matrix/matrix.hpp"
#include <list>
#include <memory>
#include <algorithm>

#include "event.hpp"

namespace gnid
{

using namespace std;
using namespace tmat;

class Scene;

/**
 * \brief A node in a scene
 */
class Node : public enable_shared_from_this<Node>
{
    public:
        Node();
        Node(const Node & other);

        /**
         * \brief Called just before the node is added to a new scene
         */
        virtual void onSceneChanged(shared_ptr<Scene> newScene) {}

        /**
         * \brief Called just before the node is added to a new parent node
         */
        virtual void onParentChanged(shared_ptr<Node> newParent) {}

        /**
         * \brief Called just before child node is added directly to this node
         */
        virtual void onChildAdded(shared_ptr<Node> child) {}

        /**
         * \brief
         *     Called just before a node is added to this node or one of its
         *     descendants
         */
        virtual void onDescendantAdded(shared_ptr<Node> child) {}

        /**
         * \brief
         *     Called just before a node is removed from this node or one of its
         *     descendants
         */
        virtual void onDescendantRemoved(shared_ptr<Node> child) {}

        /**
         * \brief Called just after a child is removed directly from this node
         */
        virtual void onChildRemoved(shared_ptr<Node> child) {}

        /**
         * \brief Called once per frame
         */
        virtual void update(float dt) {}

        /**
         * \brief Clone this node
         * \details
         *     This should be overriden by subclasses, and should simply call
         *     the copy constructor.
         */
        virtual shared_ptr<Node> clone() = 0;

        /**
         * \brief Whether the node is enabled or not
         */
        bool isActive();

        /**
         * \brief Enable or disable the node.
         */
        void setActive(bool active);

        /**
         * \brief Call the update function on the node and its descendants
         */
        void updateAll(float dt);

        /**
         * \brief Add the child node to this node
         */
        void add(shared_ptr<Node> child);

        /**
         * \brief Get or set the local matrix of the node
         */
        virtual const Matrix4f &getLocalMatrix() const;

        /**
         * \brief Get the world matrix of the node
         */
        const Matrix4f &getWorldMatrix() const;

        /**
         * \brief
         *     Calculate a new world matrix for the node given its parent's
         *     matrix
         */
        void updateWorldMatrix(const Matrix4f &prev);

        /**
         * \brief Get this parents parent node
         */
        const weak_ptr<Node> &getParent() const;

        /**
         * \brief Get the scene this node is currently associated with
         */
        const weak_ptr<Scene> &getScene() const;

        /**
         * \brief
         *     Calculate and return the position of the node from its world
         *     matrix
         */
        Vector3f position();
    private:
        list<shared_ptr<Node>> children;
        Matrix4f worldMatrix;
        bool active;
        weak_ptr<Node> parent;
        weak_ptr<Scene> scene;

        friend class Scene;

        void onSceneChangedAll(shared_ptr<Scene> newScene);
        void onDescendantAddedAll(shared_ptr<Node> child);
        void onDescendantRemovedAll(shared_ptr<Node> child);
};

} /* namespace */

#endif
