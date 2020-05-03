#ifndef SPATIALNODE_HPP
#define SPATIAL_NODE_HPP

#include "matrix.hpp"
#include "node.hpp"

using namespace tmat;

class SpatialNode : public Node
{
    private:
        Matrix4f localMatrix;
    public:
        SpatialNode();
        Matrix4f getLocalMatrix();
        void transform(Matrix4f matrix);
};

#endif

