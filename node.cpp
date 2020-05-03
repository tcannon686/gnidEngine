#include <list>
#include "scene.hpp"
#include "node.hpp"

Matrix4f Node::getLocalMatrix()
{
    return Matrix4f::identity;
}

Matrix4f Node::getWorldMatrix()
{
    return worldMatrix;
}

void Node::updateWorldMatrix(Matrix4f prev)
{
    worldMatrix = getLocalMatrix() * prev;

    for(auto it = children.begin();
        it != children.end();
        ++ it)
    {
        (*it)->updateWorldMatrix(worldMatrix);
    }
}

void Node::add(shared_ptr<Node> child)
{
    shared_ptr<Node> child_parent = child->parent.lock();
    shared_ptr<Scene> child_scene = child->scene.lock();
    shared_ptr<Scene> this_scene = scene.lock();

    if(child_parent)
    {
        child_parent->onChildRemoved(child);
        child_parent->children.remove(child);
    }
    onChildAdded(child);
    child->onParentChanged(shared_from_this());
    child->parent = shared_from_this();
    children.push_front(child);

    /*
     * If in two different scenes, call the remove callback on the previous
     * scene and the add callback on the new scene.
     */
    if(child_scene != this_scene)
    {
        if(child_scene)
            child_scene->onNodeRemoved(child);
        if(this_scene)
            this_scene->onNodeAdded(child);

        child->onSceneChanged(this_scene);
        child->scene = this_scene;
    }
}

void Node::updateAll(float dt)
{
    update(dt);
    for(auto it = begin(children);
        it != end(children);
        ++ it)
    {
        (*it)->updateAll(dt);
    }
}

bool Node::isActive()
{
    return active;
}

void Node::setActive(bool active)
{
    this->active = active;
}

Node::Node() : active(true)
{
}

