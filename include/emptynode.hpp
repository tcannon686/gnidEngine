#ifndef EMPTYNODE_HPP
#define EMPTYNODE_HPP

#include "node.hpp"

namespace gnid
{

class EmptyNode : public Node
{
    shared_ptr<Node> clone() override
    {
        return make_shared<EmptyNode>(*this);
    }
};

} /* gnid */ 

#endif /* ifndef EMPTYNODE_HPP */

