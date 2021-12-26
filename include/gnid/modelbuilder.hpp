#ifndef MODELBUILDER_HPP
#define MODELBUILDER_HPP

#include "gnid/objparser.hpp"
#include "gnid/node.hpp"
#include "gnid/matrix/matrix.hpp"

#include <memory>
#include <fstream>
#include <iostream>
#include <string.h>

namespace gnid
{

class ModelBuilder
{
public:
    ModelBuilder();
    ModelBuilder &fromObj(std::istream &stream);
    ModelBuilder &fromObj(const std::string &path);
    ModelBuilder &loadMesh(bool loadMesh = true);
    ModelBuilder &loadPhysics(bool loadPhysics = false);
    ModelBuilder &materials(
            const std::unordered_map<std::string, std::shared_ptr<Material>>
            &materials);
    ModelBuilder &materials(
            std::shared_ptr<MaterialMapping> &materials);
    ModelBuilder &translate(const tmat::Vector3f &translate);
    ModelBuilder &translate(float x, float y, float z);
    ModelBuilder &rotate(float angle, const tmat::Vector3f &axis);
    ModelBuilder &rotateX(float angle);
    ModelBuilder &rotateY(float angle);
    ModelBuilder &rotateZ(float angle);
    ModelBuilder &scale(const tmat::Vector3f &scale);
    ModelBuilder &scale(float x, float y, float z);
    std::shared_ptr<Node> build();
private:
    std::unordered_map<std::string, std::shared_ptr<Material>> materials_;
    std::shared_ptr<MaterialMapping> materialsPtr_;
    bool loadMesh_;
    bool loadPhysics_;
    std::unique_ptr<ObjParser> objParser_;
    std::ifstream stream_;
    tmat::Matrix4f transform_ = tmat::Matrix4f::identity;
};

};

#endif
