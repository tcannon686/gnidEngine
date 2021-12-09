#ifndef LIGHTNODE_HPP
#define LIGHTNODE_HPP

#include "gnid/camera.hpp"
#include "gnid/shader.hpp"
#include "matrix/matrix.hpp"
#include "gnid/node.hpp"

namespace gnid
{

/**
 * \brief A light in a scene
 */
class LightNode : public Node
{
public:
    virtual void setLight(
        int index,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<ShaderProgram> program) = 0;
    virtual void onSceneChanged(std::shared_ptr<Scene> newScene) override;
    const tmat::Vector3f &color() const;
    tmat::Vector3f &color();
private:
    tmat::Vector3f color_;
};

}; /* namespace */

#endif /* ifndef LIGHTNODE_HPP */

