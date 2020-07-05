#ifndef SPATIALNODE_HPP
#define SPATIALNODE_HPP

#include "matrix/matrix.hpp"
#include "node.hpp"

namespace gnid
{

using namespace tmat;

class SpatialNode : public Node
{
    private:
        Matrix4f localMatrix;
    public:
        SpatialNode();
        Matrix4f getLocalMatrix();
        void setLocalMatrix(Matrix4f matrix);

        /* Transform this node by the specified matrix. Return this node. */
        shared_ptr<SpatialNode> transform(Matrix4f matrix);

        shared_ptr<Node> clone() override
        {
            return make_shared<SpatialNode>(*this);
        }
};

}; /* namespace */

#endif

