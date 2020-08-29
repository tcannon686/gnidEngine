#include "spatialnode.hpp"

#include "matrix/matrix.hpp"

using namespace gnid;
using namespace tmat;

SpatialNode::SpatialNode()
    : Node(), localMatrix(Matrix4f::identity)
{
}

const Matrix4f &SpatialNode::getLocalMatrix() const
{
    return localMatrix;
}

void SpatialNode::setLocalMatrix(const Matrix4f &matrix)
{
    localMatrix = matrix;
}

void SpatialNode::transformLocal(const Matrix4f &matrix)
{
    localMatrix = matrix * localMatrix;
}

void SpatialNode::transformWorld(const Matrix4f &matrix)
{
    shared_ptr<Node> parent = getParent().lock();

    if(parent)
    {
        Matrix4f parentMatrixInverse = parent->getWorldMatrix().inverse();
        localMatrix = matrix * parentMatrixInverse * getWorldMatrix();
    }
    else
    {
        localMatrix = matrix * localMatrix;
    }
    updateWorldMatrix();
}

