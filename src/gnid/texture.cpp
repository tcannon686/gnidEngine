#include "gnid/texture.hpp"
#include "lodepng.h"
#include <cassert>

using namespace gnid;

std::shared_ptr<Texture2D> gnid::loadTexture(const std::string &path)
{
    std::vector<GLubyte> png;
    std::vector<GLubyte> image;
    unsigned int width, height;
    assert(!lodepng::load_file(png, path));
    lodepng::decode(image, width, height, png);
    return std::make_shared<Texture<2, GLubyte>>(width, height, GL_RGBA, image);
}
