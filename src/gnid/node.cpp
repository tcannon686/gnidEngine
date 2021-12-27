#include "gnid/node.hpp"

#include <list>
#include <cassert>
#include "gnid/scene.hpp"

using namespace tmat;
using namespace gnid;

const Matrix4f &Node::localMatrix() const
{
    return Matrix4f::identity;
}

const Matrix4f &Node::localMatrixInverse() const
{
    return Matrix4f::identity;
}

const Matrix4f &Node::worldMatrix() const
{
    shared_ptr<Node> p = parent.lock();

    if(p)
        return p->worldMatrix();

    return Matrix4f::identity;
}

const Matrix4f &Node::worldMatrixInverse() const
{
    shared_ptr<Node> p = parent.lock();

    if(p)
        return p->worldMatrixInverse();

    return Matrix4f::identity;
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

void Node::remove(shared_ptr<Node> child)
{
    shared_ptr<Node> child_parent = child->parent.lock();
    assert(child_parent == shared_from_this());

    children.remove(child);
    onChildRemoved(child);
    onDescendantRemovedAll(child);
    child->onAncestorRemovedAll(child_parent);
    child->onSceneChangedAll(nullptr);
}

void Node::remove()
{
    auto this_parent = parent.lock();
    assert(this_parent);
    this_parent->remove(shared_from_this());
}

void Node::add(shared_ptr<Node> child)
{
    shared_ptr<Node> child_parent = child->parent.lock();
    shared_ptr<Scene> child_scene = child->scene.lock();
    shared_ptr<Scene> this_scene = scene.lock();

    /* Remove the child from its old parent. */
    if(child_parent)
    {
        child_parent->children.remove(child);
        child_parent->onChildRemoved(child);
        child_parent->onDescendantRemovedAll(child);
        child->onAncestorRemovedAll(child_parent);
    }


    /* Add the child. */
    child->parent = shared_from_this();
    children.push_front(child);

    /* Invoke callback functions. */
    onChildAdded(child);
    onDescendantAddedAll(child);
    child->onParentChanged(child_parent);
    child->onAncestorAddedAll(shared_from_this());

    /*
     * If in two different scenes, call the remove callback on the previous
     * scene and the add callback on the new scene.
     */
    if(child_scene != this_scene)
    {
        child->onSceneChangedAll(this_scene);
    }
}

bool &Node::isActive()
{
    return isActive_;
}

Node::Node()
{
}

Node::~Node()
{
}

Node::Node(const Node &other)
    :  isActive_(other.isActive_)
{
}

void Node::cloneChildren(shared_ptr<Node> other)
{
    for(auto it = begin(other->children);
            it != end(other->children);
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
    return transform(worldMatrix(), Vector3f::zero);
}

Vector3f Node::right() const
{
    return transformDirection(worldMatrix(), Vector3f::right);
}

Vector3f Node::up() const
{
    return transformDirection(worldMatrix(), Vector3f::up);
}

Vector3f Node::forward() const
{
    return transformDirection(worldMatrix(), Vector3f::forward);
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

void Node::onAncestorAddedAll(shared_ptr<Node> ancestor)
{
    onAncestorAdded(ancestor);
    for(auto &child : children)
    {
        child->onAncestorAddedAll(ancestor);
    }
}

void Node::onAncestorRemovedAll(shared_ptr<Node> ancestor)
{
    onAncestorRemoved(ancestor);
    for(auto &child : children)
    {
        child->onAncestorRemovedAll(ancestor);
    }
}

void Node::newFrame()
{
    /* pass */
}

bool Node::moved() const
{
    shared_ptr<Node> p = getParent().lock();
    if(p)
    {
        return p->moved();
    }
    else
        return false;
}

