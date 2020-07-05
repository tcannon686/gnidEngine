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

out vec4 fragNormal;
out vec4 fragVertex;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main() {
    fragVertex = modelViewMatrix * vec4(vertex.xyz, 1);
    fragNormal = (modelViewMatrix * vec4(normal, 0));

    gl_Position = projectionMatrix * fragVertex;
}
)VERT";

static const char *frag_code = R"FRAG(
#version 440
out vec4 fragColor;
in vec4 fragNormal;
in vec4 fragVertex;

uniform int lightCount;
uniform vec4 lights[32];

void main() {
    fragColor = vec4(0, 0, 0, 1);

    for(int i = 0; i < lightCount; i ++)
    {
        vec3 l = normalize(lights[i].xyz - fragVertex.xyz);
        fragColor += vec4(1, 1, 1, 0) * dot(l, fragNormal.xyz);
    }
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

    modelViewMatrixLoc = glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
    lightCountLoc = glGetUniformLocation(program, "lightCount");

    assert(modelViewMatrixLoc != -1);
    assert(projectionMatrixLoc != -1);
    assert(lightCountLoc != -1);

    for(int i = 0; i < 32; i ++)
    {
        string uniform = "lights[" + to_string(i) + "]";

        lightsLocs[i] = glGetUniformLocation(program, uniform.c_str());
        assert(lightsLocs[i] != -1);
    }
}

void PhongShader::use()
{
    glUseProgram(program);
}

void PhongShader::setProjectionMatrix(Matrix4f matrix)
{
    GLfloat mat[16];
    matrix.toArray(mat);

    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, mat);
}

void PhongShader::setModelViewMatrix(int instance, Matrix4f matrix)
{
    GLfloat mat[16];
    matrix.toArray(mat);
    
    glUniformMatrix4fv(modelViewMatrixLoc, 1, GL_FALSE, mat);
}

int PhongShader::getMaxInstances()
{
    return 1;
}

void PhongShader::setLightCount(int count)
{
    glUniform1i(lightCountLoc, count);
}

void PhongShader::setLightPosition(int index, Vector3f position)
{
    float light[4];
    light[3] = 0;
    position.toArray(light);
    glUniform4fv(lightsLocs[index], 1, light);
}

