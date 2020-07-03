#include <string>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "phongshader.hpp"

using namespace gnid;
using namespace tmat;
using namespace std;

static const char *vert_code = R"VERT(
#version 440
in vec4 vertex;
in vec3 normal;

out vec3 fragNormal;

layout(location = 1) uniform mat4 modelViewMatrix;
layout(location = 2) uniform mat4 projectionMatrix;
void main() {
    gl_Position = projectionMatrix * modelViewMatrix * vec4(vertex.xyz, 1);
    fragNormal = (modelViewMatrix * vec4(normal, 0)).xyz;
}
)VERT";

static const char *frag_code = R"FRAG(
#version 440
out vec4 fragColor;
in vec3 fragNormal;

void main() {
    fragColor = vec4(dot(fragNormal, vec3(0, 0, 1)));
}
)FRAG";

void PhongShader::init()
{
    GLuint vert, frag;

    program = glCreateProgram();
    assert(program != 0);

    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);

    assert(vert != 0);
    assert(frag != 0);

    glShaderSource(vert, 1, &vert_code, nullptr);
    glShaderSource(frag, 1, &frag_code, nullptr);

    glCompileShader(vert);
    glCompileShader(frag);

    GLint param;

    glGetShaderiv(vert, GL_COMPILE_STATUS, &param);
    if(param != GL_TRUE)
    {
        glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &param);

        char *infoLog = new char[param];
        glGetShaderInfoLog(vert, param, NULL, infoLog);
        cerr << "vertex shader compilation failed:" << endl
            << infoLog << endl;
        assert(false);
    }

    glGetShaderiv(frag, GL_COMPILE_STATUS, &param);
    if(param != GL_TRUE)
    {
        glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &param);

        char *infoLog = new char[param];
        glGetShaderInfoLog(frag, param, NULL, infoLog);
        cerr << "fragment shader compilation failed:" << endl
            << infoLog << endl;
        delete[] infoLog;
        assert(false);
    }

    glAttachShader(program, vert);
    glAttachShader(program, frag);

    glBindAttribLocation(program, ATTRIB_LOCATION_VERTEX, "vertex");
    glBindAttribLocation(program, ATTRIB_LOCATION_NORMAL, "normal");

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &param);
    if(param != GL_TRUE)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &param);

        char *infoLog = new char[param];
        glGetProgramInfoLog(program, param, NULL, infoLog);
        cerr << "failed to link:" << endl
            << infoLog << endl;
        delete[] infoLog;
        assert(false);
    }
}

void PhongShader::use()
{
    glUseProgram(program);
}

void PhongShader::setProjectionMatrix(Matrix4f matrix)
{
    GLfloat mat[16];
    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 4; j ++)
        {
            mat[i * 4 + j] = matrix[i][j];
        }
    }

    glUniformMatrix4fv(2, 1, GL_FALSE, mat);
}

void PhongShader::setModelViewMatrix(int instance, Matrix4f matrix)
{
    GLfloat mat[16];
    for(int i = 0; i < 4; i ++)
    {
        for(int j = 0; j < 4; j ++)
        {
            mat[i * 4 + j] = matrix[i][j];
        }
    }

    glUniformMatrix4fv(1, 1, GL_FALSE, mat);
}

int PhongShader::getMaxInstances()
{
    return 1;
}

