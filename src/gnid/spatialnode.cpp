#include "gnid/spatialnode.hpp"

#include "gnid/matrix/matrix.hpp"

using namespace gnid;
using namespace tmat;

SpatialNode::SpatialNode()
    : Node(), localMatrix_(Matrix4f::identity)
{
}

const Matrix4f &SpatialNode::localMatrix() const
{
    return localMatrix_;
}

const Matrix4f &SpatialNode::localMatrixInverse() const
{
    if(shouldUpdateLocalMatrixInverse_)
    {
        localMatrixInverse_ = localMatrix().inverse();
        shouldUpdateLocalMatrixInverse_ = false;
    }
    return localMatrixInverse_;
}

Matrix4f &SpatialNode::localMatrix()
{
    shouldUpdateLocalMatrixInverse_ = true;
    shouldUpdateWorldMatrixInverse_ = true;
    moved_=  true;
    return localMatrix_;
}

const Matrix4f &SpatialNode::worldMatrix() const
{
    shared_ptr<Node> p = getParent().lock();

    if(moved() || p->moved())
    {
        worldMatrix_ = p->worldMatrix() * localMatrix();
        return worldMatrix_;
    }

    return localMatrix();
}

const Matrix4f &SpatialNode::worldMatrixInverse() const
{
    if(shouldUpdateWorldMatrixInverse_)
    {
        worldMatrixInverse_ = worldMatrix().inverse();
        shouldUpdateWorldMatrixInverse_ = false;
    }
    return worldMatrixInverse_;
}

void SpatialNode::transformLocal(const Matrix4f &matrix)
{
    localMatrix() = matrix * localMatrix_;
}

void SpatialNode::transformWorld(const Matrix4f &matrix)
{
    shared_ptr<Node> parent = getParent().lock();

    if(parent)
    {
        Matrix4f parentMatrixInverse = parent->worldMatrix().inverse();
        localMatrix() = matrix * parentMatrixInverse * worldMatrix();
    }
    else
    {
        localMatrix() = matrix * localMatrix();
    }
}

void SpatialNode::onAncestorAdded(shared_ptr<Node> ancestor)
{
    moved_ = true;
}

void SpatialNode::newFrame()
{
    moved_ = false;
}

bool SpatialNode::moved() const
{
    return moved_;
}

