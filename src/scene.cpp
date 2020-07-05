
#include <iostream>

#include "node.hpp"
#include "scene.hpp"

using namespace gnid;
using namespace std;

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
    bool hasCamera = false;

    for(auto it = begin(cameras);
        it != end(cameras);
        ++ it)
    {
        if((*it)->isActive())
        {
            renderer.render(*it);
            hasCamera = true;
        }
    }

    if(!hasCamera)
    {
        cerr << "No active cameras added to scene!" << endl;
    }
}
