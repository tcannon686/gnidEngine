
#include "matrix.hpp"
#include "spatialnode.hpp"

using namespace tmat;

SpatialNode::SpatialNode()
    : Node(), localMatrix(Matrix4f::identity)
{
}

Matrix4f SpatialNode::getLocalMatrix()
{
    return localMatrix;
}

void SpatialNode::transform(Matrix4f matrix)
{
    localMatrix = localMatrix * matrix;
}

