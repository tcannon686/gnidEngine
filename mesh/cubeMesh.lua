
VertexArray = require("wrap/VertexArray")
PointCloud = require("wrap/PointCloud")
IndexArray = require("wrap/IndexArray")

local CubeMesh = {}


local cubeVertices = {
    Vector(0.0, 0.0, 0.0, 1.0),
    Vector(0.0, 0.0, 0.0, 1.0),
    Vector(0.0, 0.0, 0.0, 1.0),
    Vector(1.0, 0.0, 0.0, 1.0),
    Vector(1.0, 0.0, 0.0, 1.0),
    Vector(1.0, 0.0, 0.0, 1.0),
    Vector(0.0, 1.0, 0.0, 1.0),
    Vector(0.0, 1.0, 0.0, 1.0),
    Vector(0.0, 1.0, 0.0, 1.0),
    Vector(1.0, 1.0, 0.0, 1.0),
    Vector(1.0, 1.0, 0.0, 1.0),
    Vector(1.0, 1.0, 0.0, 1.0),
    Vector(0.0, 0.0, 1.0, 1.0),
    Vector(0.0, 0.0, 1.0, 1.0),
    Vector(0.0, 0.0, 1.0, 1.0),
    Vector(1.0, 0.0, 1.0, 1.0),
    Vector(1.0, 0.0, 1.0, 1.0),
    Vector(1.0, 0.0, 1.0, 1.0),
    Vector(0.0, 1.0, 1.0, 1.0),
    Vector(0.0, 1.0, 1.0, 1.0),
    Vector(0.0, 1.0, 1.0, 1.0),
    Vector(1.0, 1.0, 1.0, 1.0),
    Vector(1.0, 1.0, 1.0, 1.0),
    Vector(1.0, 1.0, 1.0, 1.0)
}

local cubeNormals = {
    Vector(-1.0, 0.0, 0.0, 0.0),
    Vector(0.0, -1.0, 0.0, 0.0),
    Vector(0.0, 0.0, -1.0, 0.0),
    Vector(1.0, 0.0, 0.0, 0.0),
    Vector(0.0, -1.0, 0.0, 0.0),
    Vector(0.0, 0.0, -1.0, 0.0),
    Vector(-1.0, 0.0, 0.0, 0.0),
    Vector(0.0, 1.0, 0.0, 0.0),
    Vector(0.0, 0.0, -1.0, 0.0),
    Vector(1.0, 0.0, 0.0, 0.0),
    Vector(0.0, 1.0, 0.0, 0.0),
    Vector(0.0, 0.0, -1.0, 0.0),
    Vector(-1.0, 0.0, 0.0, 0.0),
    Vector(0.0, -1.0, 0.0, 0.0),
    Vector(0.0, 0.0, 1.0, 0.0),
    Vector(1.0, 0.0, 0.0, 0.0),
    Vector(0.0, -1.0, 0.0, 0.0),
    Vector(0.0, 0.0, 1.0, 0.0),
    Vector(-1.0, 0.0, 0.0, 0.0),
    Vector(0.0, 1.0, 0.0, 0.0),
    Vector(0.0, 0.0, 1.0, 0.0),
    Vector(1.0, 0.0, 0.0, 0.0),
    Vector(0.0, 1.0, 0.0, 0.0),
    Vector(0.0, 0.0, 1.0, 0.0)
}

local cubeIndices = {
    3, 6, 9,
    9, 6, 12,
    4, 16, 10,
    16, 22, 10,
    18, 15, 24,
    24, 21, 15,
    13, 1, 19,
    19, 1, 7,
    2, 14, 5,
    5, 14, 17,
    23, 11, 8,
    8, 20, 23
}

cubeMesh = {}
cubeMesh.vertices = cubeVertices
cubeMesh.normals = cubeNormals
cubeMesh.indices = cubeIndices
cubeMesh.indexArray = IndexArray:new(cubeIndices)

cubeMesh.pointCloud = PointCloud:new {
    vertex = VertexArray:new(cubeVertices),
    normal = VertexArray:new(cubeNormals),
}

return cubeMesh
