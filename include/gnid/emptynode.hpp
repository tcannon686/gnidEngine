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
        return std::make_shared<EmptyNode>(*this);
    }
};

} /* gnid */ 

#endif /* ifndef EMPTYNODE_HPP */

