#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

#include "gnid/camera.hpp"
#include "gnid/shader.hpp"
#include "gnid/matrix/matrix.hpp"
#include "gnid/lightnode.hpp"

namespace gnid
{

/**
 * \brief A directional light source
 */
class DirectionalLight : public LightNode
{
public:
    DirectionalLight();

    /**
     * \brief Returns the direction towards the light source
     */
    tmat::Vector3f &direction();

    std::shared_ptr<Node> clone() override;

    void setLight(
        int index,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<ShaderProgram> program) override;
private:
    tmat::Vector3f direction_;
};

} /* namespace */

#endif /* ifndef DIRECTIONALLIGHT_HPP */
