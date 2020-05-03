#ifndef NODE_HPP
#define NODE_HPP

#include "matrix.hpp"
#include <list>
#include <memory>
#include <algorithm>

using namespace std;
using namespace tmat;

class Scene;

class Node : public enable_shared_from_this<Node>
{
    private:
        list<shared_ptr<Node>> children;
        Matrix4f worldMatrix;
        bool active;

        friend class Scene;

    protected:
        weak_ptr<Node> parent;
        weak_ptr<Scene> scene;

    public:
        Node();

        virtual void onSceneChanged(shared_ptr<Scene> newScene) {}
        virtual void onParentChanged(shared_ptr<Node> newParent) {}
        virtual void onChildAdded(shared_ptr<Node> child) {}
        virtual void onChildRemoved(shared_ptr<Node> child) {}

        virtual void update(float dt) {}

        bool isActive();
        void setActive(bool active);

        void updateAll(float dt);

        void add(shared_ptr<Node> child);

        virtual Matrix4f getLocalMatrix();
        Matrix4f getWorldMatrix();

        void updateWorldMatrix(Matrix4f prev);
};
#endif
