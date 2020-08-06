#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include <set>
#include "node.hpp"
#include "emptynode.hpp"
#include "camera.hpp"
#include "box.hpp"
#include "matrix/matrix.hpp"
#include "renderer.hpp"
#include "collider.hpp"

namespace gnid
{

using namespace tmat;

class Scene : public enable_shared_from_this<Scene>
{
    public:
        typedef list<shared_ptr<Camera>> CameraList;

        CameraList cameras;
        const shared_ptr<Node> root;
        Renderer renderer;

        Scene()
            : root(make_shared<EmptyNode>())
        {
        }

        void init();

        void render();
        void update(float dt);
        void updateWorldMatrix();

        void addCollider(shared_ptr<Collider> collider);
        void removeCollider(shared_ptr<Collider> collider);
    private:
        friend class Node;
        list<shared_ptr<Collider>> colliders;
        set<Collision> collisions;
};

}; /* namespace */

#endif

