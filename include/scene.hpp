#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "node.hpp"
#include "camera.hpp"
#include "matrix/matrix.hpp"
#include "renderer.hpp"

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
            : root(make_shared<Node>())
        {
        }

        void init();

        void render();
        void update(float dt);
        void updateWorldMatrix();

    private:
        friend class Node;
};

}; /* namespace */

#endif

