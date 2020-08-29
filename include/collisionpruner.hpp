#ifndef COLLISIONPRUNER_HPP
#define COLLISIONPRUNER_HPP

#include <memory>
#include <vector>

#include "collider.hpp"

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

    CollisionPruner()
    {
    }

    virtual ~CollisionPruner()
    {
    }

    /**
     * \brief
     *     Finds the nodes whose bounding boxes are overlapping and stores them
     *     in the given list
     *
     * \details
     *     The list **will not** be cleared. Applications should clear the list
     *     before passing it in to this function if needed. Each pair of nodes
     *     should only be included once.
     */
    virtual void listOverlappingNodes(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list) const = 0;

    /**
     * \brief Adds the given node to be pruned
     */
    virtual void add(std::shared_ptr<Collider> collider) = 0;

    /**
     * \brief Removes the given node
     */
    virtual void remove(std::shared_ptr<Collider> collider) = 0;

    /**
     * \brief Update the pruner
     *
     * \details
     *     This function should be called after all nodes have updated their
     *     world matrices.
     */
    virtual void update() = 0;
};

} /* namespace */

#endif
