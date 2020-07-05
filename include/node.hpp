#ifndef NODE_HPP
#define NODE_HPP

#include "matrix/matrix.hpp"
#include <list>
#include <memory>
#include <algorithm>

namespace gnid
{

using namespace std;
using namespace tmat;

class Scene;

class Node : public enable_shared_from_this<Node>
{
    private:
        list<shared_ptr<Node>> children;
        Matrix4f worldMatrix;
        bool active;
        weak_ptr<Node> parent;
        weak_ptr<Scene> scene;

        friend class Scene;

        void onSceneChangedAll(shared_ptr<Scene> newScene);
    public:
        Node();
        Node(const Node & other);

        virtual void onSceneChanged(shared_ptr<Scene> newScene) {}
        virtual void onParentChanged(shared_ptr<Node> newParent) {}
        virtual void onChildAdded(shared_ptr<Node> child) {}
        virtual void onChildRemoved(shared_ptr<Node> child) {}

        virtual void update(float dt) {}

        virtual shared_ptr<Node> clone() = 0;

        bool isActive();
        void setActive(bool active);

        void updateAll(float dt);

        /* Add a node to this node. Return this node. */
        shared_ptr<Node> add(shared_ptr<Node> child);

        virtual Matrix4f getLocalMatrix();
        Matrix4f getWorldMatrix();

        void updateWorldMatrix(Matrix4f prev);

        const weak_ptr<Node> &getParent() const;
        const weak_ptr<Scene> &getScene() const;

        Vector4f position();
};

} /* namespace */

#endif
