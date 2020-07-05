
#include "matrix/matrix.hpp"
#include "spatialnode.hpp"

using namespace gnid;
using namespace tmat;

SpatialNode::SpatialNode()
    : Node(), localMatrix(Matrix4f::identity)
{
}

Matrix4f SpatialNode::getLocalMatrix()
{
    return localMatrix;
}

void SpatialNode::setLocalMatrix(Matrix4f matrix)
{
    localMatrix = matrix;
}

shared_ptr<SpatialNode> SpatialNode::transform(Matrix4f matrix)
{
    localMatrix = localMatrix * matrix;
    return static_pointer_cast<SpatialNode>(shared_from_this());
}

