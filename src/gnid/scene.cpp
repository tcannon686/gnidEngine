#include "gnid/scene.hpp"

#include <iostream>
#include <set>

#include "gnid/renderernode.hpp"
#include "gnid/emptynode.hpp"
#include "gnid/node.hpp"
#include "gnid/spatialnode.hpp"
#include "gnid/collider.hpp"
#include "gnid/collisionevent.hpp"

using namespace tmat;
using namespace gnid;
using namespace std;

Scene::Scene()
    : root(make_shared<EmptyNode>()),
      kdTree(make_shared<KdTree>()),
      pruner(kdTree)
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

    /* Update the boxes for all the colliders. */
    for(auto &collider : colliders)
        collider->calcBox();
    pruner.update();
    
    vector<pair<shared_ptr<Collider>, shared_ptr<Collider>>> overlappingNodes;
    pruner.listOverlappingNodes(overlappingNodes);

    /* Find overlapping colliders. */
    for(auto &overlappingPair : overlappingNodes)
    {
        auto &a = overlappingPair.first;
        auto &b = overlappingPair.second;

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

            /* Move the objects away from each other. */
            auto as = a->findAncestorByType<SpatialNode>();
            auto bs = b->findAncestorByType<SpatialNode>();

            if(as)
                as->transformWorld(getTranslateMatrix(-overlap * 0.5f));

            if(bs)
                bs->transformWorld(getTranslateMatrix(overlap * 0.5f));

            as->updateWorldMatrixAll();
            bs->updateWorldMatrixAll();
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
}

void Scene::updateWorldMatrix()
{
    root->updateWorldMatrixAll();
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
    /* TODO */
}

void Scene::unregisterNode(shared_ptr<Rigidbody> rigidbody)
{
    /* TODO */
}

