#include "gnid/modelbuilder.hpp"
#include "gnid/emptynode.hpp"
#include "gnid/spatialnode.hpp"
#include <iostream>

#include <cassert>

using namespace std;
using namespace gnid;
using namespace tmat;

ModelBuilder::ModelBuilder()
{
}

ModelBuilder &ModelBuilder::fromObj(std::istream &stream)
{
    objParser_ = std::unique_ptr<ObjParser>(new ObjParser(stream));
    return *this;
}

ModelBuilder &ModelBuilder::fromObj(const std::string &path)
{
    stream_ = std::ifstream(path);
    objParser_ = std::unique_ptr<ObjParser>(new ObjParser(stream_));
    return *this;
}

ModelBuilder &ModelBuilder::loadMesh(bool loadMesh)
{
    loadMesh_ = true;
    return *this;
}

ModelBuilder &ModelBuilder::loadPhysics(bool loadMesh)
{
    loadPhysics_ = true;
    return *this;
}

ModelBuilder &ModelBuilder::materials(const MaterialMapping &materials)
{
    materials_ = materials;
    return *this;
}

ModelBuilder &ModelBuilder::materials(std::shared_ptr<MaterialMapping> &materials)
{
    materialsPtr_ = materials;
    return *this;
}

ModelBuilder &ModelBuilder::scale(const tmat::Vector3f &scale)
{
    transform_ = getTranslateMatrix(scale) * transform_;
    return *this;
}

ModelBuilder &ModelBuilder::scale(float x, float y, float z)
{
    return scale({ x, y, z });
}

ModelBuilder &ModelBuilder::translate(const tmat::Vector3f &translate)
{
    transform_ = getTranslateMatrix(translate) * transform_;
    return *this;
}

ModelBuilder &ModelBuilder::translate(float x, float y, float z)
{
    return translate({ x, y, z });
}

ModelBuilder &ModelBuilder::rotate(float angle, const tmat::Vector3f &axis)
{
    transform_ = getRotateMatrix(angle, axis) * transform_;
    return *this;
}

ModelBuilder &ModelBuilder::rotateX(float angle)
{
    return rotate(angle, Vector3f::right);
}

ModelBuilder &ModelBuilder::rotateY(float angle)
{
    return rotate(angle, Vector3f::up);
}

ModelBuilder &ModelBuilder::rotateZ(float angle)
{
    return rotate(angle, Vector3f::forward);
}

std::shared_ptr<Node> ModelBuilder::build()
{
    assert(loadMesh_ || loadPhysics_);
    assert(objParser_);
    objParser_->parse();
    const auto &materials = materialsPtr_ ? *materialsPtr_ : materials_;
    std::shared_ptr<Node> node;
    if(loadMesh_ && !loadPhysics_)
    {
        node = objParser_->buildRendererNode(materials);
    }
    else if (!loadMesh_ && loadPhysics_)
    {
        const auto node = objParser_->buildPhysicsNode();
    }
    else
    {
        node = objParser_->buildPhysicsNode();
        node->add(objParser_->buildRendererNode(materials));
    }

    if (!(transform_ == Matrix4f::identity))
    {
        const auto root = make_shared<SpatialNode>();
        root->transformWorld(transform_);
        root->add(node);
        return root;
    }
    else
    {
        return node;
    }
}
