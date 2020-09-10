#ifndef SPATIALNODE_HPP
#define SPATIALNODE_HPP

#include "gnid/matrix/matrix.hpp"
#include "gnid/node.hpp"

namespace gnid
{

/**
 * \brief A node whose transformation can be controlled
 */
class SpatialNode : public Node
{
public:
    SpatialNode();

    const tmat::Matrix4f &localMatrix() const override;
    const tmat::Matrix4f &worldMatrix() const override;
    const tmat::Matrix4f &localMatrixInverse() const;
    const tmat::Matrix4f &worldMatrixInverse() const;

    tmat::Matrix4f &localMatrix();

    /**
     * \brief Transforms this node's local matrix by the given matrix
     *
     * \details
     *     Transforms in local space. The new local matrix is calculated as
     *     matrix * localMatrix().
     */
    void transformLocal(const tmat::Matrix4f &matrix);

    /**
     * \brief Transforms this node's world matrix by the specified matrix
     *
     * \details
     *     This does not update the world matrix of this nodes descendants.
     *     To do that, call the updateWorldMatrix function. The new world
     *     matrix is calculated as matrix * worldMatrix().
     */
    void transformWorld(const tmat::Matrix4f &matrix);

    bool moved() const override;

    void newFrame() override;

    void onAncestorAdded(std::shared_ptr<Node> ancestor) override;

    std::shared_ptr<Node> clone() override
    {
        auto ret = std::make_shared<SpatialNode>(*this);
        ret->cloneChildren(shared_from_this());
        return ret;
    }

private:
    tmat::Matrix4f localMatrix_;
    mutable tmat::Matrix4f worldMatrix_;
    mutable tmat::Matrix4f localMatrixInverse_;
    mutable tmat::Matrix4f worldMatrixInverse_;
    mutable bool shouldUpdateLocalMatrixInverse_ = true;
    mutable bool shouldUpdateWorldMatrixInverse_ = true;
    bool moved_;
};

}; /* namespace */

#endif

