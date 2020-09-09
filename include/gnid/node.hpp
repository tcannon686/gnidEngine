#ifndef NODE_HPP
#define NODE_HPP

#include "matrix/matrix.hpp"
#include <list>
#include <memory>
#include <algorithm>

#include "gnid/event.hpp"

namespace gnid
{

class Scene;

/**
 * \brief A node in a scene
 */
class Node : public std::enable_shared_from_this<Node>
{
    public:
        Node();
        virtual ~Node();

        Node(const Node &other);

        /**
         * \brief Called just before the node is added to a new scene
         */
        virtual void onSceneChanged(std::shared_ptr<Scene> newScene) {}

        /**
         * \brief Called just before the node is added to a new parent node
         */
        virtual void onParentChanged(std::shared_ptr<Node> newParent) {}

        /**
         * \brief Called just before child node is added directly to this node
         */
        virtual void onChildAdded(std::shared_ptr<Node> child) {}

        /**
         * \brief
         *     Called just before a node is added to this node or one of its
         *     descendants
         */
        virtual void onDescendantAdded(std::shared_ptr<Node> child) {}

        /**
         * \brief
         *     Called just before a node is removed from this node or one of its
         *     descendants
         */
        virtual void onDescendantRemoved(std::shared_ptr<Node> child) {}

        /**
         * \brief Called just after a child is removed directly from this node
         */
        virtual void onChildRemoved(std::shared_ptr<Node> child) {}

        /**
         * \brief Called once per frame
         */
        virtual void update(float dt) {}

        /**
         * \brief Clone this node
         * \details
         *     This should be overriden by subclasses, and should call the copy
         *     constructor as well as cloneChildren.
         */
        virtual std::shared_ptr<Node> clone() = 0;

        /**
         * \brief
         *     Clone the descendants of the given node and add them to this
         *     node
         *
         * \details
         *     This should be called by the clone method.
         */
        void cloneChildren(std::shared_ptr<Node> other);

        /**
         * \brief Whether the node is enabled or not
         */
        bool &isActive();

        /**
         * \brief Call the update function on the node and its descendants
         */
        void updateAll(float dt);

        /**
         * \brief Add the child node to this node
         *
         * \details
         *     If the child already had a parent, it will be removed from its
         *     parent node.
         */
        void add(std::shared_ptr<Node> child);

        /**
         * \brief Get or set the local matrix of the node
         */
        virtual const tmat::Matrix4f &getLocalMatrix() const;

        /**
         * \brief Get the world matrix of the node
         */
        const tmat::Matrix4f &getWorldMatrix() const;

        /**
         * \brief
         *     Calculates the world matrix for this node from its local matrix
         */
        void updateWorldMatrix();

        /**
         * \brief
         *     Calculates the world matrix for this node and its descendants
         *     from its local matrix
         *
         * \details
         *     This function will be called after all nodes have been updated
         */
        void updateWorldMatrixAll();

        /**
         * \brief Get this parents parent node
         */
        const std::weak_ptr<Node> &getParent() const;

        /**
         * \brief Get the scene this node is currently associated with
         */
        const std::weak_ptr<Scene> &getScene() const;

        /**
         * \brief
         *     Calculate and return the position of the node from its world
         *     matrix
         */
        tmat::Vector3f position() const;

        /**
         * \brief
         *    Calculate and return the right direction of the node from its
         *    world matrix
         */
        tmat::Vector3f right() const;

        /**
         * \brief
         *    Calculate and return the up direction of the node from its world
         *    matrix
         */
        tmat::Vector3f up() const;

        /**
         * \brief
         *    Calculate and return the forward direction of the node from its
         *    world matrix
         */
        tmat::Vector3f forward() const;

        /**
         * \brief
         *     Find the first ancestor of this node with the given type, or this
         *     node if it has the type
         */
        template<class T>
        std::shared_ptr<T> findAncestorByType()
        {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(
                    shared_from_this());
            
            /* If this is the right type, return this. */
            if(t)
                return t;
            /* Otherwise repeat at this node's parent. */
            else
            {
                std::shared_ptr<Node> p = parent.lock();
                if(p)
                    return p->findAncestorByType<T>();
                else
                    return nullptr;
            }
        }

        /**
         * \brief
         *     Find the first child of this node with the given type, or this
         *     node if it has the type
         * \details
         *     This function takes \f$O(n)\f$ time where \f$n\f$ is the number
         *     of children.
         */
        template<class T>
        std::shared_ptr<T> findChildByType()
        {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(
                    shared_from_this());
            
            /* If this is the right type, return this. */
            if(t)
                return t;
            /* Otherwise, find at each child. */
            else
            {
                for(auto &child : children)
                {
                    t = std::dynamic_pointer_cast<T>(child);
                    if(t)
                        return t;
                }
            }
            return nullptr;
        }

    private:
        std::list<std::shared_ptr<Node>> children;
        tmat::Matrix4f worldMatrix;
        bool isActive_;
        std::weak_ptr<Node> parent;
        std::weak_ptr<Scene> scene;

        friend class Scene;

        void onSceneChangedAll(std::shared_ptr<Scene> newScene);
        void onDescendantAddedAll(std::shared_ptr<Node> child);
        void onDescendantRemovedAll(std::shared_ptr<Node> child);
};

} /* namespace */

#endif
