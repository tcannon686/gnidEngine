#ifndef KDTREE_HPP
#define KDTREE_HPP

#include <memory>
#include <stack>
#include <vector>

#include "gnid/collisionpruner.hpp"

namespace gnid
{

/* Forward declarations. */
class Node;
class Collider;

/**
 * \brief A k-D tree for partitioning nodes in space
 */
class KdTree
{
public:
    KdTree();
    KdTree(const KdTree &other);
    KdTree(KdTree &&other);

    /**
     * \brief Return the bounding box for the KdTree node
     */
    const Box &box() const { return box_; }

    /**
     * \brief Update the KdTree from the new positions of the colliders
     *
     * \details
     *     This function should be called at least once per frame. Returns true
     *     if the k-D tree node changed its bounding box.
     */
    bool update();
    
    /**
     * \brief Add a collider to the KdTree
     *
     * \details
     *     This function will add the given collider to the KdTree, regenerating
     *     as needed
     */
    void add(std::shared_ptr<Collider> collider);

    /**
     * \brief Clear all nodes from the tree
     */
    void clear();

     /**
      * \brief
      *     List the nodes whose bounding boxes overlap and store them in the
      *     list
      */
    void listOverlappingNodes(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list) const;

    /**
     * \brief List all of the nodes in the tree and add them to the list
     */
    void listAllNodes(std::vector<std::shared_ptr<Collider>> &list) const;

    /**
     * \brief Regenerate the tree
     */
    void regenerate();

    /**
     * \brief
     *     Returns the maximum number of colliders allowed in each leaf node
     *
     * \details
     *     If a leaf node has more than this amount of colliders, it will be
     *     regenerated.
     */
    const unsigned int &maxNodesPerLeaf() const { return maxNodesPerLeaf_; }

    /**
     * \brief
     *     The maximum amount the median should shift before regenerating the
     *     tree
     */
    float &maxShift() { return maxShift_; }

    void print(int indent = 0);

private:
    const unsigned int maxNodesPerLeaf_;
    unsigned int axisIndex;
    float median;
    float maxShift_;
    unsigned int totalNodes;    /* The total number of nodes in the tree. */
    bool needsUpdate_;

    mutable bool visited_;

    std::vector<std::shared_ptr<Collider>> nodes;
    Box box_;
    std::unique_ptr<KdTree> left, right;

    static void listOverlappingNodes(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list,
            const KdTree &first,
            const KdTree &second);

    /**
     * \brief clearVisited() should be called before calling this function
     */
    void listOverlappingNodesRecursive(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list) const;

    void generate();
    
    /**
     * \brief Clear the visited flag for all nodes in the tree.
     */
    void clearVisited() const;
};

/**
 * \brief A collision pruner based on a k-D tree
 */
class KdTreePruner : public CollisionPruner
{
public:
    /**
     * \brief Create the pruner from the given k-D tree
     */
    KdTreePruner(std::shared_ptr<KdTree> kdTree);

    void listOverlappingNodes(
            std::vector<
                std::pair<std::shared_ptr<Collider>, std::shared_ptr<Collider>>
            > &list) const override;

    void add(std::shared_ptr<Collider>) override;
    void remove(std::shared_ptr<Collider>) override;

    /**
     * \brief Update the k-D tree
     * 
     * \details
     *     This function should not be called if the k-D tree is updated from
     *     another source, for example using the KdTree.update() function.
     */
    void update() override;
private:
    const std::shared_ptr<KdTree> kdTree_;
};

} /* namespace */

#endif
