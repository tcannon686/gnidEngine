#ifndef LIGHTNODE_HPP
#define LIGHTNODE_HPP

namespace gnid
{

class LightNode : public Node
{
public:
    void onSceneChanged(shared_ptr<Scene> newScene) override;
    shared_ptr<Node> clone() override;

    float &distance() { return _distance; }
private:
    float _distance = 1.0f;
};

}; /* namespace */

#endif /* ifndef LIGHTNODE_HPP */

