#ifndef DIRECTIONALLIGHT_HPP
#define DIRECTIONALLIGHT_HPP

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
private:
    tmat::Vector3f direction_;
};

} /* namespace */

#endif /* ifndef DIRECTIONALLIGHT_HPP */
