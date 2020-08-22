
#include <iostream>
#include <set>

#include "emptynode.hpp"
#include "node.hpp"
#include "spatialnode.hpp"
#include "scene.hpp"
#include "collider.hpp"
#include "collisionevent.hpp"

using namespace gnid;
using namespace std;

Scene::Scene() : root(make_shared<EmptyNode>())
{
}

void Scene::init()
{
    root->scene = shared_from_this();
}

void Scene::update(float dt)
{
    root->updateAll(dt);

    updateWorldMatrix();

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

                /* Collision already exists, send onCollisionStay event. */
                if(!item.second)
                {
                    item.first->overlap_ = overlap;
                    item.first->visited_ = true;

                    /* TODO send event */
                }
                /* New collision, send the onCollisionEnter event. */
                else
                {
                    cout << "collision enter " << a << " " << b << " " << overlap << endl;
                    item.first->overlap_ = overlap;
                    item.first->visited_ = true;

                    /* TODO send event */
                }
            }
        }
    }

    /* Find unvisited collision objects. */
    for(auto it = begin(collisions);
        it != end(collisions);
        /* pass */)
    {
        /* The objects are not colliding anymore, call onCollisionExit. */
        if(it->visited_ == false)
        {
            auto a = it->colliders()[0];
            auto b = it->colliders()[1];

            /* TODO notify */
            cout << "collision exit " << a << " " << b << " " << overlap << endl;

            collisions.erase(it ++);
        }
        else
            ++ it;
    }

    /* Move colliding objects away from each other. */
    /* Iterate through all collisions */
    for(auto it = begin(collisions);
        it != end(collisions);
        ++ it)
    {
        auto a = it->colliders()[0];
        auto b = it->colliders()[1];
        auto overlap = it->overlap();

        /* Find the spatial nodes for the colliders. */
        auto as = a->findAncestorByType<SpatialNode>();
        auto bs = b->findAncestorByType<SpatialNode>();

        /* Move them away from eachother. */
        if((as->position() - bs->position()).dot(overlap) < 0)
        {
            as->transform(
                    getTranslateMatrix(
                        transformDirection(as->getWorldMatrix().inverse(), -overlap)));
            bs->transform(
                    getTranslateMatrix(
                        transformDirection(bs->getWorldMatrix().inverse(), overlap)));
        }
        else
        {
            as->transform(
                    getTranslateMatrix(
                        transformDirection(as->getWorldMatrix().inverse(), overlap * 0.5f)));
            bs->transform(getTranslateMatrix(
                        transformDirection(bs->getWorldMatrix().inverse(), -overlap * 0.5f)));
        }
    }

    updateWorldMatrix();
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

void Scene::registerNode(shared_ptr<Collider> collider)
{
    colliders.push_front(collider);
}

void Scene::unregisterNode(shared_ptr<Collider> collider)
{
    colliders.remove(collider);
}

void Scene::registerNode(shared_ptr<Camera> camera)
{
    cameras.push_front(camera);
}

void Scene::unregisterNode(shared_ptr<Camera> camera)
{
    cameras.remove(camera);
}

void Scene::registerNode(shared_ptr<Rigidbody> rigidbody)
{
    /* TODO */
}

void Scene::unregisterNode(shared_ptr<Rigidbody> rigidbody)
{
    /* TODO */
}

