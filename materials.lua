local Material = require("wrap/Material")
local textures = require("textures")
local shaders = require("shaders")

local materials = {
    default = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0, 1),
            enableDiffuseTexture = false,
            shadeless = false
        }),
    red1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0, 0),
            enableDiffuseTexture = false,
            shadeless = false
        }),
    green1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0, 1, 0),
            enableDiffuseTexture = false,
            shadeless = false
        }),
    blue1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0, 0, 1),
            enableDiffuseTexture = false,
            shadeless = false
        }),
    flash1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0.75, 0),
            enableDiffuseTexture = false,
            shadeless = true
        }),
    smoke1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0.25, 0.25, 0.25),
            enableDiffuseTexture = false,
            shadeless = false
        }),
    test1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.test1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    brick1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.brick1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    concrete1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.concrete1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    metal1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0.5, 0.5, 0.5, 1.0),
            enableDiffuseTexture = false,
            shadeless = false
        }),
    road1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.road1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    sand1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.sand1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    snow1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.snow1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    wood1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    wood2 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood2,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    wood3 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood3,
            enableDiffuseTexture = true,
            shadeless = false
        }),
    sphereMonster1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.sphereMonster1,
            enableDiffuseTexture = true,
            shadeless = false
        }),
}

return materials
