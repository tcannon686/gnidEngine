#include "gnid/modelbuilder.hpp"
#include "gnid/emptynode.hpp"
#include <iostream>

#include <cassert>

using namespace gnid;

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

ModelBuilder &ModelBuilder::materials(
        const std::unordered_map<std::string, std::shared_ptr<Material>>
        &materials)
{
    materials_ = materials;
    return *this;
}

ModelBuilder &ModelBuilder::materials(
        std::shared_ptr<
            std::unordered_map<std::string, std::shared_ptr<Material>>
        > &materials)
{
    materialsPtr_ = materials;
    return *this;
}

std::shared_ptr<Node> ModelBuilder::build()
{
    assert(loadMesh_ || loadPhysics_);
    assert(objParser_);
    objParser_->parse();
    const auto &materials = materialsPtr_ ? *materialsPtr_ : materials_;
    if(loadMesh_ && !loadPhysics_)
    {
        return objParser_->buildRendererNode(materials);
    }
    else if (!loadMesh_ && loadPhysics_)
    {
        return objParser_->buildPhysicsNode();
    }
    else
    {
        auto root = objParser_->buildPhysicsNode();
        root->add(objParser_->buildRendererNode(materials));
        return root;
    }
}
