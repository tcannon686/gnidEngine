#include "collisionpruner.hpp"

namespace gnid
{

/**
 * \brief A collision pruner based on a k-D tree
 */
class KdTreePruner : public CollisionPruner
{
public:
    /**
     * \brief Create the pruner from the given k-D tree
     * TODO implement KdTree
     */
    KdTreePruner(std::shared_ptr<KdTree> kdTree);

    void listOverlappingNodes(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list) override const;

    void add(std::shared_ptr<Collider>) override;
    void remove(std::shared_ptr<Collider>) override;

    void update() override;
};

} /* namespace */

