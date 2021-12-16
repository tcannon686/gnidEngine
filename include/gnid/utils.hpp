#ifndef UTILS_HPP
#define UTILS_HPP

#include "gnid/glad/glad.h"
#include <GLFW/glfw3.h>

namespace gnid
{

template<typename T>
GLenum glType() = delete;
template<> GLenum glType<GLfloat>();
template<> GLenum glType<GLdouble>();
template<> GLenum glType<GLbyte>();
template<> GLenum glType<GLubyte>();
template<> GLenum glType<GLshort>();
template<> GLenum glType<GLushort>();
template<> GLenum glType<GLint>();
template<> GLenum glType<GLuint>();

} /* namespace */

#endif
