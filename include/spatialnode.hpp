#ifndef SPATIALNODE_HPP
#define SPATIALNODE_HPP

#include "matrix/matrix.hpp"
#include "node.hpp"

namespace gnid
{

using namespace tmat;

/**
 * \brief A node whose transformation can be controlled
 */
class SpatialNode : public Node
{
    public:
        SpatialNode();

        const Matrix4f &getLocalMatrix() const override;

        /**
         * \brief Override the local matrix for the node
         */
        void setLocalMatrix(const Matrix4f &matrix);

        /**
         * \brief Transforms this node's local matrix by the given matrix
         *
         * \details
         *     Transforms in local space. The new local matrix is calculated as
         *     matrix * getLocalMatrix().
         */
        void transformLocal(const Matrix4f &matrix);

        /**
         * \brief Transforms this node's world matrix by the specified matrix
         *
         * \details
         *     This does not update the world matrix of this nodes descendants.
         *     To do that, call the updateWorldMatrix function. The new world
         *     matrix is calculated as matrix * getWorldMatrix().
         */
        void transformWorld(const Matrix4f &matrix);

        shared_ptr<Node> clone() override
        {
            return make_shared<SpatialNode>(*this);
        }

    private:
        Matrix4f localMatrix;
};

}; /* namespace */

#endif

