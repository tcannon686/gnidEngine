#ifndef AMBIENTLIGHT_HPP
#define AMBIENTLIGHT_HPP

#include "gnid/camera.hpp"
#include "gnid/shader.hpp"
#include "gnid/lightnode.hpp"
#include "matrix/matrix.hpp"

namespace gnid
{

/**
 * \brief An ambient light
 */
class AmbientLight : public LightNode
{
public:
    AmbientLight();

    std::shared_ptr<Node> clone() override;

    void setLight(
        int index,
        std::shared_ptr<Camera> camera,
        std::shared_ptr<ShaderProgram> program) override;
};

}

#endif /* ifndef AMBIENTLIGHT_HPP */
