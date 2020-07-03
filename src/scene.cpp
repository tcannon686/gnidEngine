
#include "node.hpp"
#include "scene.hpp"

using namespace gnid;

void Scene::init()
{
    root->scene = shared_from_this();
}

void Scene::update(float dt)
{
    root->updateAll(dt);
}

void Scene::updateWorldMatrix()
{
    root->updateWorldMatrix(Matrix4f::identity);
}

void Scene::render()
{
    for(auto it = begin(cameras);
        it != end(cameras);
        ++ it)
    {
        if((*it)->isActive())
        {
            renderer.render(*it);
        }
    }
}

void Scene::onNodeAdded(shared_ptr<Node> node)
{
    shared_ptr<Camera> camera;
    
    /* If a camera was added add it to the camera list. */
    if(camera = dynamic_pointer_cast<Camera>(node))
    {
        cameras.push_front(camera);
    }
}

void Scene::onNodeRemoved(shared_ptr<Node> node)
{
}
