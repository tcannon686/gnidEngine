#include <list>
#include <cassert>
#include "scene.hpp"
#include "node.hpp"

using namespace gnid;

const Matrix4f &Node::getLocalMatrix() const
{
    return Matrix4f::identity;
}

const Matrix4f &Node::getWorldMatrix() const
{
    return worldMatrix;
}

void Node::updateWorldMatrix(const Matrix4f &prev)
{
    worldMatrix = prev * getLocalMatrix();

    for(auto it = children.begin();
        it != children.end();
        ++ it)
    {
        (*it)->updateWorldMatrix(worldMatrix);
    }
}

void Node::onSceneChangedAll(shared_ptr<Scene> newScene)
{
    onSceneChanged(newScene);
    scene = newScene;

    // Update children.
    for(auto it = children.begin();
        it != children.end();
        ++ it)
    {
        (*it)->onSceneChangedAll(newScene);
    }
}

void Node::add(shared_ptr<Node> child)
{
    shared_ptr<Node> child_parent = child->parent.lock();
    shared_ptr<Scene> child_scene = child->scene.lock();
    shared_ptr<Scene> this_scene = scene.lock();

    /* Remove the child from its old parent. */
    if(child_parent)
    {
        child_parent->onChildRemoved(child);
        /*child_parent->onDescendantRemovedAll(child);*/
        child_parent->children.remove(child);
    }

    /* Invoke callback functions. */
    onChildAdded(child);
    /*onDescendantAddedAll(child);*/

    child->onParentChanged(shared_from_this());

    /* Add the child. */
    child->parent = shared_from_this();
    children.push_front(child);

    /*
     * If in two different scenes, call the remove callback on the previous
     * scene and the add callback on the new scene.
     */
    if(child_scene != this_scene)
    {
        child->onSceneChangedAll(this_scene);
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

Node::Node(const Node &other)
{
    // Clone children.
    for(auto it = begin(other.children);
        it != end(other.children);
        ++ it)
    {
        add((*it)->clone());
    }
}

const weak_ptr<Node> &Node::getParent() const
{
    return parent;
}

const weak_ptr<Scene> &Node::getScene() const
{
    return scene;
}

Vector3f Node::position() const
{
    return transform(worldMatrix, Vector3f::zero);
}

void Node::onDescendantAddedAll(shared_ptr<Node> child)
{
    shared_ptr<Node> parent;

    /* Invoke the callback function. */
    onDescendantAdded(child);

    parent = getParent().lock();
    if(parent)
    {
        parent->onDescendantAddedAll(child);
    }
}

void Node::onDescendantRemovedAll(shared_ptr<Node> child)
{
    shared_ptr<Node> parent;

    /* Invoke the callback function. */
    onDescendantRemoved(child);

    parent = getParent().lock();
    if(parent)
    {
        parent->onDescendantRemovedAll(child);
    }
}

