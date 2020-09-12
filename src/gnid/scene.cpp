#include "gnid/scene.hpp"

#include <iostream>
#include <set>

#include "gnid/renderernode.hpp"
#include "gnid/emptynode.hpp"
#include "gnid/node.hpp"
#include "gnid/spatialnode.hpp"
#include "gnid/collider.hpp"
#include "gnid/collision.hpp"
#include "gnid/collisionevent.hpp"
#include "gnid/rigidbody.hpp"

using namespace tmat;
using namespace gnid;
using namespace std;

Scene::Scene()
    : root(make_shared<EmptyNode>()),
      kdTree(make_shared<KdTree>()),
      pruner(kdTree),
      gravity_ { 0.0f, -9.8f, 0.0f }
{
}

void Scene::init()
{
    root->scene = shared_from_this();
}

void Scene::handleCollision(
        shared_ptr<Collider> a,
        shared_ptr<Collider> b,
        Vector3f overlap)
{
    Collision collision(a, b, overlap);
    auto item = collisions.insert(collision);

    /* Collision already exists, send collisionStayed event. */
    if(!item.second)
    {
        item.first->overlap_ = overlap;
        item.first->visited_ = true;
        
        item.first->colliders()[0]->notifyCollisionObservers(
                *item.first,
                item.first->colliders()[0]->collisionStayedObservers);
        item.first->colliders()[1]->notifyCollisionObservers(
                item.first->swapped(),
                item.first->colliders()[1]->collisionStayedObservers);
    }
    /* New collision, send the onCollisionEnter event. */
    else
    {
        item.first->colliders()[0]->notifyCollisionObservers(
                *item.first,
                item.first->colliders()[0]->collisionEnteredObservers);
        item.first->colliders()[1]->notifyCollisionObservers(
                item.first->swapped(),
                item.first->colliders()[1]->collisionEnteredObservers);
    }

    /* Move the objects away from each other if necessary. */
    auto as = a->findAncestorByType<Rigidbody>();
    auto bs = b->findAncestorByType<Rigidbody>();

    float lenOverlap = overlap.magnitude();

    /* Nothing to do if no overlap. */
    if(lenOverlap == 0)
        return;

    /* Nothing to do if either is a trigger. */
    if(a->isTrigger() || b->isTrigger())
        return;

    /* If as is a rigidbody. */
    if(as && as->isActive())
    {
        float lenVelocityA = as->velocity_.magnitude();
        /* If they are both rigidbodies, move them away from eachother. */
        if(bs && bs->isActive())
        {
            float lenVelocityB = bs->velocity_.magnitude();
            as->transformWorld(getTranslateMatrix(-overlap * 0.5f));
            bs->transformWorld(getTranslateMatrix(overlap * 0.5f));

            /* Calculate the new velocities. */
            if(lenVelocityA > 0)
            {
                as->velocity_ += as->velocity_
                    * (as->velocity_.dot(-overlap.normalized())
                            / lenVelocityA);
            }

            if(lenVelocityB > 0)
            {
                bs->velocity_ += bs->velocity_ * (bs->velocity_.dot(overlap)
                            / (lenVelocityB * lenOverlap));
            }
        }
        /* If just as is a rigidbody, move it away. */
        else
        {
            as->transformWorld(getTranslateMatrix(-overlap));

            /* Calculate the new velocity. */
            if(lenVelocityA > 0)
            {
                as->velocity_ += as->velocity_ * (as->velocity_.dot(-overlap)
                            / (lenVelocityA * lenOverlap));
            }
        }
    }
    /* If as is not a rigidbody. */
    else
    {
        /* If bs is a rigidbody, move it away. */
        if(bs && bs->isActive())
        {
            float lenVelocityB = bs->velocity_.magnitude();
            bs->transformWorld(getTranslateMatrix(overlap));

            if(lenVelocityB > 0)
            {
                bs->velocity_ += bs->velocity_ * (bs->velocity_.dot(overlap)
                            / (lenVelocityB * lenOverlap));
            }
        }
        /* Otherwise neither are rigidbodies, so nothing to do. */
    }
}

void Scene::update(float dt)
{
    root->newFrameAll();
    root->updateAll(dt);

    Vector3f overlap;

    /* Update the rigid bodies. */
    for(auto &rb : rigidbodies)
    {
        rb->addImpulse(gravity_ * rb->mass() * dt);
        rb->physicsUpdate(dt);
    }


    /* Update the boxes for all the colliders. */
    for(auto &collider : colliders)
        collider->calcBox();
    pruner.update();
    
    vector<pair<shared_ptr<Collider>, shared_ptr<Collider>>> overlappingNodes;
    pruner.listOverlappingNodes(overlappingNodes);

    /* Mark all collisions as unvisited. */
    for(auto it = begin(collisions);
            it != end(collisions);
            ++ it)
    {
        it->visited_ = false;
    }

    /* Find overlapping colliders. */
    for(auto &overlappingPair : overlappingNodes)
    {
        auto &a = overlappingPair.first;
        auto &b = overlappingPair.second;

        /* We only need to check if either collider is active. */
        if(a->isActive() && b->isActive())
        {
            auto as = a->findAncestorByType<Rigidbody>();
            auto bs = b->findAncestorByType<Rigidbody>();

            if(as || bs)
            {
                Vector3f initialAxis = Vector3f::right;
                /* If there is overlap, handle the collision. */
                if(a->getOverlap(overlap, initialAxis, b))
                {
                    handleCollision(a, b, overlap);
                }
            }
        }
    }

    /* Find unvisited collision objects. */
    for(auto it = begin(collisions);
            it != end(collisions);
            /* pass */)
    {
        /* The objects are not colliding anymore, call collisionExited. */
        if(!it->visited_)
        {
            auto a = it->colliders()[0];
            auto b = it->colliders()[1];

            a->notifyCollisionObservers(
                    *it,
                    a->collisionExitedObservers);
            b->notifyCollisionObservers(
                    it->swapped(),
                    b->collisionExitedObservers);

            collisions.erase(it ++);
        }
        else
            ++ it;
    }
}

void Scene::render()
{
    double startTime = glfwGetTime();
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
    pruner.add(collider);
}

void Scene::unregisterNode(shared_ptr<Collider> collider)
{
    colliders.remove(collider);
    pruner.remove(collider);
}

void Scene::registerNode(shared_ptr<Camera> camera)
{
    cameras.push_front(camera);
}

void Scene::unregisterNode(shared_ptr<Camera> camera)
{
    cameras.remove(camera);
}

void Scene::registerNode(shared_ptr<RendererNode> rendererNode)
{
    Binding binding(rendererNode->material, rendererNode->mesh, rendererNode);
    renderer.add(binding);
}

void Scene::unregisterNode(shared_ptr<RendererNode> rendererNode)
{
    Binding binding(rendererNode->material, rendererNode->mesh, rendererNode);
    renderer.remove(binding);
}

void Scene::registerNode(shared_ptr<LightNode> lightNode)
{
    renderer.add(lightNode);
}

void Scene::unregisterNode(shared_ptr<LightNode> lightNode)
{
    renderer.remove(lightNode);
}

void Scene::registerNode(shared_ptr<Rigidbody> rigidbody)
{
    rigidbodies.push_back(rigidbody);
}

void Scene::unregisterNode(shared_ptr<Rigidbody> rigidbody)
{
    rigidbodies.remove(rigidbody);
}

