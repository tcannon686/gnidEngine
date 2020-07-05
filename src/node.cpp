#include <list>
#include <cassert>
#include "scene.hpp"
#include "node.hpp"

using namespace gnid;

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

shared_ptr<Node> Node::add(shared_ptr<Node> child)
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
        child->onSceneChangedAll(this_scene);
    }

    return shared_from_this();
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

shared_ptr<Node> Node::clone()
{
    return make_shared<Node>(*this);
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

Vector4f Node::position()
{
    return (worldMatrix * Vector4f { 0, 0, 0, 1 });
}

