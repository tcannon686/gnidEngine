#ifndef EMPTYNODE_HPP
#define EMPTYNODE_HPP

#include "gnid/node.hpp"

namespace gnid
{

/**
 * \brief An empty node
 */
class EmptyNode : public Node
{
    std::shared_ptr<Node> clone() override
    {
        auto ret = std::make_shared<EmptyNode>(*this);
        ret->cloneChildren(shared_from_this());
        return ret;
    }
};

} /* gnid */ 

#endif /* ifndef EMPTYNODE_HPP */

