local Material = require("wrap/Material")
local textures = require("textures")
local shaders = require("shaders")

local materials = {
    default = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0, 1),
            enableDiffuseTexture = false
        }),
    red1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(1, 0, 0),
            enableDiffuseTexture = false
        }),
    green1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0, 1, 0),
            enableDiffuseTexture = false
        }),
    blue1 = Material:new(
        shaders.default,
        {
            diffuseColor = Vector(0, 0, 1),
            enableDiffuseTexture = false
        }),
    test1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.test1,
            enableDiffuseTexture = true
        }),
    brick1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.brick1,
            enableDiffuseTexture = true
        }),
    concrete1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.concrete1,
            enableDiffuseTexture = true
        }),
    road1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.road1,
            enableDiffuseTexture = true
        }),
    sand1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.sand1,
            enableDiffuseTexture = true
        }),
    snow1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.snow1,
            enableDiffuseTexture = true
        }),
    wood1 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood1,
            enableDiffuseTexture = true
        }),
    wood2 = Material:new(
        shaders.default,
        {
            diffuseTexture = textures.wood2,
            enableDiffuseTexture = true
        }),
}

return materials
