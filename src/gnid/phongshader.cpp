#include "gnid/phongshader.hpp"

#include <string>
#include <cassert>

#include "gnid/glad/glad.h"
#include <GLFW/glfw3.h>

#include "gnid/camera.hpp"
#include "gnid/lightnode.hpp"
#include "gnid/directionallight.hpp"
#include "gnid/pointlight.hpp"
#include "gnid/ambientlight.hpp"

using namespace gnid;
using namespace tmat;
using namespace std;

static const char *vert_code = R"VERT(
#version 440
in vec4 vertex;
in vec3 normal;
in vec2 texCo;

out vec3 fragNormal;
out vec3 fragVertex;
out vec2 fragTexCo;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main() {
    fragVertex = (modelViewMatrix * vec4(vertex.xyz, 1.0)).xyz;
    fragNormal = (modelViewMatrix * vec4(normal, 0.0)).xyz;
    fragTexCo = texCo;

    gl_Position = projectionMatrix * vec4(fragVertex, 1.0);
}
)VERT";

static const char *frag_code = R"FRAG(
#version 440
out vec4 fragColor;
in vec3 fragNormal;
in vec3 fragVertex;
in vec2 fragTexCo;

/* Light definitions. */
uniform int lightCount;

/*
 * If w component is greater than zero, represents a point light with the
 * position xyz. If w component equals zero, represents a directional light with
 * the direction xyz.
 */
uniform vec4 lights[32];
uniform vec3 lightColors[32];

uniform vec3 ambientColor;

/* Material definition. */
uniform vec3 diffuseColor;
uniform sampler2D diffuseTexture;
uniform float diffuseMix;
uniform vec3 specularColor;
uniform float specularExponent;

void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 n = normalize(fragNormal);
    vec3 albedo = mix(
        diffuseColor,
        texture(diffuseTexture, fragTexCo).xyz,
        diffuseMix);

    for(int i = 0; i < lightCount; i ++)
    {
        /*
         * Use the direction to the surface to the light if the light is a point
         * light, else use the light direction.
         */
        vec3 l = (lights[i].w > 0.0)
                ? normalize(lights[i].xyz - fragVertex)
                : normalize(lights[i].xyz);

        /* Calculate attenuation for point light. */
        float attenuation = (lights[i].w > 0.0)
                ? clamp(lights[i].w / dot(l, l), 0, 1)
                : 1.0;

        float kS = pow(
                clamp(dot(normalize(l - normalize(fragVertex)), n), 0.0, 1.0),
                specularExponent);
        
        float kD = clamp(dot(l, n), 0.0, 1.0);

        vec3 diffuse = albedo * lightColors[i] * attenuation * kD;
        vec3 specular = specularColor * lightColors[i] * attenuation * kS;

        fragColor += vec4(diffuse + specular, 0.0);
    }
    vec3 ambient = albedo * ambientColor;
    fragColor = clamp(fragColor + vec4(ambient, 0.0), 0.0, 1.0);
    fragColor.w = 1.0;
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
    glBindAttribLocation(program, ATTRIB_LOCATION_TEXCO, "texCo");

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
    diffuseColorLoc = glGetUniformLocation(program, "diffuseColor");
    diffuseMixLoc = glGetUniformLocation(program, "diffuseMix");
    diffuseTextureLoc = glGetUniformLocation(program, "diffuseTexture");
    ambientColorLoc = glGetUniformLocation(program, "ambientColor");
    specularColorLoc = glGetUniformLocation(program, "specularColor");
    specularExponentLoc = glGetUniformLocation(program, "specularExponent");

    assert(modelViewMatrixLoc != -1);
    assert(projectionMatrixLoc != -1);
    assert(lightCountLoc != -1);
    assert(diffuseColorLoc != -1);
    assert(diffuseMixLoc != -1);
    assert(diffuseTextureLoc != -1);
    assert(ambientColorLoc != -1);
    assert(specularColorLoc != -1);
    assert(specularExponentLoc != -1);

    for(int i = 0; i < 32; i ++)
    {
        string uniform = "lights[" + to_string(i) + "]";
        string uniformColor = "lightColors[" + to_string(i) + "]";

        lightsLocs[i] = glGetUniformLocation(program, uniform.c_str());
        lightColorsLocs[i] = glGetUniformLocation(program, uniformColor.c_str());
        assert(lightsLocs[i] != -1);
        assert(lightColorsLocs[i] != -1);
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

void PhongShader::setLight(
        int index,
        shared_ptr<Camera> camera,
        shared_ptr<DirectionalLight> light)
{
    /* Calculate the light direction. */
    Vector3f position = transformDirection(
            camera->viewMatrix(), light->direction());

    float uniform[4];
    position.toArray(uniform);
    uniform[3] = 0.0f;

    glUniform4fv(lightsLocs[index], 1, uniform);

    /* Set the color. */
    const auto &color = light->color();
    glUniform3f(lightColorsLocs[index], color[0], color[1], color[2]);
}

void PhongShader::setLight(
        int index,
        shared_ptr<Camera> camera,
        shared_ptr<PointLight> light)
{
    /* Calculate the light position. */
    Vector3f position = transform(camera->viewMatrix(), light->position());

    float uniform[4];
    position.toArray(uniform);

    /* Store the distance in the last component. */
    uniform[3] = light->distance();

    glUniform4fv(lightsLocs[index], 1, uniform);

    /* Set the color. */
    const auto &color = light->color();
    glUniform3f(lightColorsLocs[index], color[0], color[1], color[2]);
}

void PhongShader::setLight(
        shared_ptr<Camera> camera,
        shared_ptr<AmbientLight> light)
{
    const auto &color = light->color();
    glUniform3f(ambientColorLoc, color[0], color[1], color[2]);
}

void PhongShader::setDiffuseMix(float mix)
{
    glUniform1f(diffuseMixLoc, mix);
}

void PhongShader::setDiffuseColor(Vector3f &diffuseColor)
{
    glUniform3f(
            diffuseColorLoc,
            diffuseColor[0],
            diffuseColor[1],
            diffuseColor[2]);
}

void PhongShader::setDiffuseTexture(std::shared_ptr<Texture2D> texture)
{
    if (texture) {
        texture->bind();
        glUniform1i(diffuseTextureLoc, 0);
    }
}

void PhongShader::setSpecularColor(Vector3f &specularColor)
{
    glUniform3f(
            specularColorLoc,
            specularColor[0],
            specularColor[1],
            specularColor[2]);
}

void PhongShader::setSpecularExponent(float exponent)
{
    glUniform1f(specularExponentLoc, exponent);
}

void PhongMaterial::bind()
{
    shader_->setDiffuseColor(diffuseColor_);
    shader_->setDiffuseMix(diffuseMix_);
    shader_->setDiffuseTexture(diffuseTexture_);
    shader_->setSpecularColor(specularColor_);
    shader_->setSpecularExponent(specularExponent_);
}

PhongMaterial::Builder::Builder()
{
}

PhongMaterial::Builder &PhongMaterial::Builder::shader(
        std::shared_ptr<PhongShader> shader)
{
    shader_ = shader;
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::diffuse(
        const tmat::Vector3f &diffuse)
{
    diffuseColor_ = diffuse;
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::diffuse(
        float r, float g, float b)
{
    diffuseColor_ = Vector3f { r, g, b };
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::specular(
        float r, float g, float b)
{
    specularColor_ = Vector3f { r, g, b };
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::shininess(
        float exponent)
{
    specularExponent_ = exponent;
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::specular(
        const tmat::Vector3f &specular, float exponent)
{
    specularColor_ = specular;
    specularExponent_ = exponent;
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::diffuse(
        std::shared_ptr<Texture2D> diffuseTexture)
{
    diffuseTexture_ = diffuseTexture;
    diffuseMix_ = 1.0f;
    return *this;
}

PhongMaterial::Builder &PhongMaterial::Builder::mix(
        float diffuseTextureMix)
{
    diffuseMix_ = diffuseTextureMix;
    return *this;
}

std::shared_ptr<PhongMaterial> PhongMaterial::Builder::build()
{
    assert(shader_);
    return std::shared_ptr<PhongMaterial>(new PhongMaterial(
            shader_,
            diffuseColor_,
            diffuseTexture_,
            diffuseMix_,
            specularColor_,
            specularExponent_));
}
