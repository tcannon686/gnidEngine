#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "node.hpp"
#include "camera.hpp"
#include "matrix.hpp"
#include "renderer.hpp"

using namespace tmat;

class Scene : public enable_shared_from_this<Scene>
{
    private:
        list<shared_ptr<Camera>> cameras;
        void onNodeAdded(shared_ptr<Node> node);
        void onNodeRemoved(shared_ptr<Node> node);
        friend class Node;
    public:
        const shared_ptr<Node> root;
        Renderer renderer;

        Scene()
            : root(make_shared<Node>())
        {
        }

        void init();

        void render();
        void update(float dt);
        void updateWorldMatrix();
};

#endif

