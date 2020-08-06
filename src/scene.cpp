
#include <iostream>
#include <set>

#include "node.hpp"
#include "scene.hpp"
#include "collider.hpp"
#include "collisionevent.hpp"

using namespace gnid;
using namespace std;

void Scene::init()
{
    root->scene = shared_from_this();
}

void Scene::update(float dt)
{
    root->updateAll(dt);

    Vector3f overlap;

    /* Mark all collisions as unvisited. */
    for(auto it = begin(collisions);
        it != end(collisions);
        ++ it)
    {
        it->visited_ = false;
    }

    /* Find overlapping colliders. */
    for(shared_ptr<Collider> a : colliders)
    {
        for(shared_ptr<Collider> b : colliders)
        {
            if(a == b)
                continue;

            /* If there is overlap, create collision object. */
            if(a->getOverlap(overlap, b))
            {
                auto item = collisions.emplace(a, b, overlap);
                cout << a << " " << b << " " << overlap << endl;

                /* Collision already exists, send onCollisionStay event. */
                if(!item.second)
                {
                    /*item.first->overlap_ = overlap;
                    item.first->visited_ = true;*/

                    /* TODO send event */
                }
                /* New collision, send the onCollisionEnter event. */
                else
                {
                    item.first->overlap_ = overlap;
                    item.first->visited_ = true;

                    /* TODO send event */
                }
            }
        }
    }

    /* Find unvisited collision objects. */
    for(auto it = begin(collisions);
        it != end(collisions);)
    {
        /* The objects are not colliding anymore, call onCollisionExit. */
        if(it->visited_ == false)
        {
            /* auto a = it->colliders()[0];
            auto b = it->colliders()[1];*/

            /* TODO notify */

            collisions.erase(it ++);
        }
        else
            ++ it;
    }
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

void Scene::addCollider(shared_ptr<Collider> box)
{
    colliders.push_front(box);
}

void Scene::removeCollider(shared_ptr<Collider> box)
{
    colliders.remove(box);
}

