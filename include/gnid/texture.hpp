#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

#include "gnid/utils.hpp"
#include "gnid/glad/glad.h"
#include <GLFW/glfw3.h>

namespace gnid
{

template<int D, typename T>
class Texture
{
};

template<typename T>
class Texture<2, T>
{
public:
    Texture(GLsizei width, GLsizei height, GLuint format, std::vector<T> data)
        : data_(data), width_(width), height_(height), format_(format)
    {
    }
    void bind();
    void init();
private:
    GLuint id_;
    GLsizei width_, height_;
    GLenum format_;
    std::vector<T> data_;
    void flipY();
    void halfSize();
};

template<typename T>
void Texture<2, T>::flipY()
{
    assert(format_ == GL_RGBA);
    const GLsizei components = 4;
    for (GLsizei j = 0; j < height_ / 2; j ++)
    {
        for (GLsizei i = 0; i < width_; i ++)
        {
            for (GLsizei k = 0; k < components; k ++)
            {
                std::swap(
                    data_[(i + j * width_) * components + k],
                    data_[(i + (height_ - j - 1) * width_) * components + k]);
            }
        }
    }
}

template<typename T>
void Texture<2, T>::halfSize()
{
    assert(format_ == GL_RGBA);
    const GLsizei components = 4;
    GLsizei index = 0;
    auto ow = width_;
    width_ /= 2;
    height_ /= 2;
    for (GLsizei j = 0; j < height_; j ++)
    {
        for (GLsizei i = 0; i < width_; i ++)
        {
            for (GLsizei k = 0; k < components; k ++)
            {
                auto oi = i * 2, oj = j * 2;
                auto average = (
                    data_[(oi + oj * ow) * components + k]
                    + data_[(oi + 1 + oj * ow) * components + k]
                    + data_[(oi + (oj + 1) * ow) * components + k]
                    + data_[(oi + 1 + (oj + 1) * ow) * components + k]) / 4;
                data_[index] = average;
                index ++;
            }
        }
    }
    assert(index == width_ * height_ * components);
    data_.resize(index);
}

template<typename T>
void Texture<2, T>::init()
{
    glGenTextures(1, &id_);
    assert(id_ != 0);
    bind();

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);

    flipY();
    GLint level = 0;
    while (width_ > 0 && height_ > 0) {
        glTexImage2D(
            GL_TEXTURE_2D,
            level,
            format_,
            width_,
            height_,
            0,
            format_,
            glType<T>(),
            reinterpret_cast<void *>(data_.data())
        );
        level ++;
        halfSize();
    }
}

template<typename T>
void Texture<2, T>::bind()
{
    assert(id_ != 0);
    glBindTexture(GL_TEXTURE_2D, id_);
}

typedef Texture<2, GLubyte> Texture2D;
std::shared_ptr<Texture2D> loadTexture(const std::string &path);

}; /* namespace */

#endif
