#ifndef BVH_HPP
#define BVH_HPP

#include "collider.hpp"
#include <memory>
#include <vector>

namespace gnid
{

/**
 * \brief A generic collision pruner
 *
 * \details
 *     This is an abstract class for pruning collisions based on the bounding
 *     boxes of shapes.
 */
class CollisionPruner
{
public:

    /**
     * \brief
     *     Finds the nodes whose bounding boxes are overlapping and stores them
     *     in the given list
     *
     * \details
     *     The list **will not** be cleared. Applications should clear the list
     *     before passing it in to this function if needed. Each pair of nodes
     *     should only be included once. The first pointer in the pair should
     *     always be less than the second.
     */
    virtual void listOverlappingNodes(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list) = 0;

    /**
     * \brief Adds the given node to be pruned
     */
    virtual void add(std::shared_ptr<Collider>);

    /**
     * \brief Removes the given node
     */
    virtual void remove(std::shared_ptr<Collider>);

    /**
     * \brief Update the pruner
     *
     * \details
     *     This function should be called after all nodes have updated their
     *     world matrices.
     */
    virtual void update();
};

} /* namespace */
