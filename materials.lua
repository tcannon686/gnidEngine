local Material = require("wrap/Material")
local textures = require("textures")
local shaders = require("shaders")

local materials = {
    default = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0, 1),
            enableDiffuseTexture = false,
            shadeless = false,
            specularColor = Vector.zero
        }),
    arm1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.arm1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    red1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0, 0),
            enableDiffuseTexture = false,
            shadeless = false,
            specularColor = Vector.zero
        }),
    green1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0, 1, 0),
            enableDiffuseTexture = false,
            shadeless = false,
            specularColor = Vector.zero
        }),
    blue1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0, 0, 1),
            enableDiffuseTexture = false,
            shadeless = false,
            specularColor = Vector.zero
        }),
    light1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 1, 0),
            enableDiffuseTexture = false,
            shadeless = true,
            specularColor = Vector.zero
        }),
    flash1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0.75, 0),
            enableDiffuseTexture = false,
            shadeless = true,
            specularColor = Vector.zero
        }),
    smoke1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0.25, 0.25, 0.25),
            enableDiffuseTexture = false,
            shadeless = false,
            specularColor = Vector.zero
        }),
    test1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.test1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    brick1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.brick1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    concrete1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.concrete1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    dirt1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.dirt1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    metal1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0.25, 0.25, 0.25, 1.0),
            enableDiffuseTexture = false,
            shadeless = false,
            specularColor = Vector.one,
            specularExponent = 32
        }),
    road1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.road1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    sand1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.sand1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    snow1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.snow1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    stone1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.stone1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    wood1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    wood2 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood2,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    wood3 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood3,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
    monospace1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.monospace1,
            enableDiffuseTexture = true,
            shadeless = true,
            specularColor = Vector.zero
        }),
    objectSpawn1 = Material:new(
        shaders.default,
        {
            enableDiffuseTexture = false,
            shadeless = true,
            diffuseColor = Vector(1.0, 0.0, 1.0),
            specularColor = Vector.zero
        }),
    playerSpawn1 = Material:new(
        shaders.default,
        {
            enableDiffuseTexture = false,
            shadeless = true,
            diffuseColor = Vector(0.0, 1.0, 0.0),
            specularColor = Vector.zero
        }),
    sphereMonster1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.sphereMonster1,
            enableDiffuseTexture = true,
            shadeless = false,
            specularColor = Vector.zero
        }),
}

return materials
