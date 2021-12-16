#include "gnid/utils.hpp"

template<> GLenum gnid::glType<GLfloat>() { return GL_FLOAT; }
template<> GLenum gnid::glType<GLdouble>() { return GL_DOUBLE; }
template<> GLenum gnid::glType<GLbyte>() { return GL_BYTE; }
template<> GLenum gnid::glType<GLubyte>() { return GL_UNSIGNED_BYTE; }
template<> GLenum gnid::glType<GLshort>() { return GL_SHORT; }
template<> GLenum gnid::glType<GLushort>() { return GL_UNSIGNED_SHORT; }
template<> GLenum gnid::glType<GLint>() { return GL_INT; }
template<> GLenum gnid::glType<GLuint>() { return GL_UNSIGNED_INT; }
