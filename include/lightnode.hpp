#ifndef LIGHTNODE_HPP
#define LIGHTNODE_HPP

namespace gnid
{

class LightNode : public Node
{
public:
    void onSceneChanged(shared_ptr<Scene> newScene);
};

}; /* namespace */

#endif /* ifndef LIGHTNODE_HPP */

