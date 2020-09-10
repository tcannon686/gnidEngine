#include "gnid/kdtree.hpp"

#include <iostream>
#include <cassert>

using namespace std;
using namespace tmat;
using namespace gnid;

KdTree::KdTree() : maxNodesPerLeaf_(1), maxShift_(0.5f)
{
}

KdTree::KdTree(const KdTree &other)
    : maxNodesPerLeaf_(other.maxNodesPerLeaf_),
      axisIndex(other.axisIndex),
      median(other.median),
      maxShift_(other.maxShift_),
      totalNodes(other.totalNodes),
      nodes(other.nodes)
{
    if(other.left)
        left = unique_ptr<KdTree>(new KdTree(*other.left));
    if(other.right)
        right = unique_ptr<KdTree>(new KdTree(*other.right));
}

KdTree::KdTree(KdTree &&other)
    : maxNodesPerLeaf_(other.maxNodesPerLeaf_),
      axisIndex(other.axisIndex),
      median(other.median),
      totalNodes(other.totalNodes)
{
    swap(nodes, other.nodes);
    swap(left, other.left);
    swap(right, other.right);
}

void KdTree::print(int indent)
{
    for(int i = 0; i < indent; i ++)
    {
        cout << " ";
    }
    cout << box().min() << " " << box().max() << endl;
    if(left)
    {
        assert(right);
        left->print(indent + 1);
        right->print(indent + 1);
    }
}

bool KdTree::update()
{
    /* If we are at an inner node. */
    if(nodes.size() == 0)
    {
        assert(left && right);

        bool updated = false;

        /* Regenerate if the maximum allowed shift is met. */
        if(abs(box_.center()[axisIndex] - median) > maxShift_)
        {
            regenerate();
            updated = true;
        }
        else
        {
            /* Update the box from the sub nodes. */
            bool updatedLeft = left->update();
            bool updatedRight = right->update();

            if(updatedLeft || updatedRight)
            {
                box_.clear();
                box_.add(left->box());
                box_.add(right->box());
                updated = true;
            }
        }

        return updated;
    }
    /* If we are at a leaf node. */
    else
    {
        /* If we have too many nodes, generate the tree. */
        if(static_cast<unsigned int>(nodes.size()) > maxNodesPerLeaf_)
        {
            generate();
            return true;
        }
        /* Otherwise just update the box from the nodes. */
        else
        {
            bool updated = false;
            for(auto node : nodes)
            {
                if(node->moved())
                {
                    updated = true;
                }
            }

            if(updated)
            {
                box_.clear();
                for(auto node : nodes)
                {
                    box_.add(node->box());
                }
            }
            return updated;
        }
    }
}

void KdTree::add(std::shared_ptr<Collider> collider)
{
    /* Add to the total node count. */
    totalNodes += 1;
    /*
     * If we are a leaf node, add to the list of nodes.
     */
    if(!left)
    {
        assert(!right);

        nodes.push_back(collider);
    }
    /* Otherwise determine where to place the node. */
    else
    {
        assert(left && right);

        if(collider->box().center()[axisIndex] < median)
            left->add(collider);
        else
            right->add(collider);
    }
}

void KdTree::clear()
{
    axisIndex = 0;
    median = 0;
    totalNodes = 0;
    nodes.clear();
    left = nullptr;
    right = nullptr;
    box_.clear();
}

void KdTree::listOverlappingNodes(
        vector<pair<shared_ptr<Collider>, shared_ptr<Collider>>> &list) const
{
    clearVisited();
    listOverlappingNodesRecursive(list);
}

void KdTree::listOverlappingNodesRecursive(
        vector<pair<shared_ptr<Collider>, shared_ptr<Collider>>> &list) const
{
    /* Do not continue if the node has already been visited. */
    if(visited_)
        return;
    visited_ = true;

    /*
     * If the node is an inner node, list the overlaps between its child nodes.
     */
    if(nodes.size() == 0)
    {
        assert(left && right);
        listOverlappingNodes(list, *left, *right);
    }
    /*
     * If the node is a leaf node, find the overlaps between its colliders
     */
    else
    {
        assert(!left && !right);
        if(maxNodesPerLeaf_ != 1)
        {
            for(auto &n1 : nodes)
            {
                for(auto &n2 : nodes)
                {
                    if(n1 != n2)
                    {
                        if(n1->box().overlaps(n2->box()))
                        {
                            list.emplace_back(n1, n2);
                        }
                    }
                }
            }
        }
    }
}

void KdTree::listOverlappingNodes(
        vector<pair<shared_ptr<Collider>, shared_ptr<Collider>>> &list,
        const KdTree &first,
        const KdTree &second)
{
    /* If the nodes overlap. */
    if(first.box_.overlaps(second.box_))
    {
        /* If the nodes are both inner nodes, check their children. */
        if(first.nodes.size() == 0 && second.nodes.size() == 0)
        {
            assert(first.left && second.right);

            listOverlappingNodes(list, *first.left,  *second.right);
            listOverlappingNodes(list, *first.right, *second.left);
            listOverlappingNodes(list, *first.left,  *second.left);
            listOverlappingNodes(list, *first.right, *second.right);
        }
        /*
         * If the first one is a leaf node but the second is an inner node,
         * check the first node against the second node's children.
         */
        else if(first.nodes.size() != 0 && second.nodes.size() == 0)
        {
            assert(!first.left && !first.right);
            assert(second.left && second.right);

            listOverlappingNodes(list, first, *second.left);
            listOverlappingNodes(list, first, *second.right);
        }
        /*
         * If the second one is a leaf node but the first is an inner node,
         * check the second node against the first node's children.
         */
        else if(second.nodes.size() != 0 && first.nodes.size() == 0)
        {
            assert(!second.left && !second.right);
            assert(first.left && first.right);

            listOverlappingNodes(list, second, *first.left);
            listOverlappingNodes(list, second, *first.right);
        }
        /*
         * Otherwise, they are both leaf nodes, check the colliders they
         * contain.
         */
        else
        {
            assert(first.nodes.size() != 0 && second.nodes.size() != 0);
            assert(!first.left && !first.right && !second.left && !second.right);

            for(auto &n1 : first.nodes)
            {
                for(auto &n2 : second.nodes)
                {
                    if(n1->box().overlaps(n2->box()))
                    {
                        list.emplace_back(n1, n2);
                    }
                }
            }
        }
    }

    first.listOverlappingNodesRecursive(list);
    second.listOverlappingNodesRecursive(list);
}

void KdTree::listAllNodes(vector<shared_ptr<Collider>> &list) const
{
    /* If the node is a leaf node. */
    if(nodes.size() > 0)
    {
        /* Add this nodes nodes to the list. */
        list.insert(end(list), begin(nodes), end(nodes));
    }
    /* If the node is an inner node. */
    else
    {
        assert(left && right);

        left->listAllNodes(list);
        right->listAllNodes(list);
    }
}

void KdTree::regenerate()
{
    /* If the node is a leaf node. */
    if(nodes.size() > 0)
    {
        generate();
    }
    /*
     * If the node is an inner node, collect all of the colliders and re-add
     * them.
     */
    else
    {
        left->listAllNodes(nodes);
        right->listAllNodes(nodes);

        left = nullptr;
        right = nullptr;
        box_.clear();
        totalNodes = nodes.size();

        generate();
    }
}

void KdTree::clearVisited() const
{
    visited_ = false;
    if(left)
    {
        assert(right);
        left->clearVisited();
        right->clearVisited();
    }
}

void KdTree::generate()
{
    assert(left == nullptr);
    assert(right == nullptr);

    /*
     * The box that contains all of the centers. We need to split using this box
     * to avoid cases where the median of the bounding box contains the centers
     * on one side.
     */
    Box centerBox;

    /* Calculate the bounds of the nodes. */
    box_.clear();
    for(auto node : nodes)
    {
        box_.add(node->box());
        centerBox.add(node->box().center());
    }

    /* Find the longest axis. */
    Vector3f size = centerBox.size();
    unsigned int longAxisIndex = 0;
    float longAxisLength = size[longAxisIndex];

    for(int i = 1; i < 3; i ++)
    {
        if(size[i] > longAxisLength)
        {
            longAxisLength = size[i];
            longAxisIndex = i;
        }
    }

    /* Split along the longest axis. */
    axisIndex = longAxisIndex;
    median = centerBox.center()[axisIndex];

    /* If we are at the correct number of nodes, stop generating. */
    if(static_cast<unsigned int>(nodes.size()) <= maxNodesPerLeaf_)
    {
        return;
    }

    /* Create the left and right subtrees. */
    left = make_unique<KdTree>();
    right = make_unique<KdTree>();

    /* Split the nodes based on their center. */
    for(auto node : nodes)
    {
        if(node->box().center()[axisIndex] < median)
            left->add(node);
        else
            right->add(node);
    }

    assert(left->nodes.size() > 0 && right->nodes.size() > 0);
    nodes.clear();

    left->generate();
    right->generate();
    median = box_.center()[longAxisIndex];
}

KdTreePruner::KdTreePruner(std::shared_ptr<KdTree> kdTree)
    : kdTree_(kdTree)
{
}

void KdTreePruner::listOverlappingNodes(
        vector<pair<shared_ptr<Collider>, shared_ptr<Collider>>> &list) const
{
    kdTree_->listOverlappingNodes(list);
}

void KdTreePruner::add(shared_ptr<Collider> collider)
{
    kdTree_->add(collider);
}

void KdTreePruner::remove(shared_ptr<Collider> collider)
{
    vector<shared_ptr<Collider>> colliders;

    /* Get all of the colliders. */
    kdTree_->listAllNodes(colliders);

    /* Regenerate the tree excluding the given collider. */
    kdTree_->clear();
    for(auto &c : colliders)
    {
        if(c != collider)
        {
            kdTree_->add(c);
        }
    }
}

void KdTreePruner::update()
{
    kdTree_->update();
}

