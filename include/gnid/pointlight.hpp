#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "gnid/lightnode.hpp"

namespace gnid
{

/**
 * \brief A point light
 */
class PointLight : public LightNode
{
public:
    PointLight();

    /**
     * \brief The distance at which the light should start to fall off
     */
    float &distance();

    std::shared_ptr<Node> clone() override;

    void setLight(
        int index,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<ShaderProgram> program) override;

private:
    float distance_ = 1.0;
};

} /* namespace */

#endif /* ifndef POINTLIGHT_HPP */
