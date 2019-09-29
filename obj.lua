local PointCloud = require("wrap/PointCloud")
local Mesh = require("wrap/Mesh")
local VertexArray = require("wrap/VertexArray")
local IndexArray = require("wrap/IndexArray")
local materials = require("materials")

local obj = {}

local cmds = {}
cmds.v = function(state, str)
    local v = Vector()
    local index = 1
    for num in string.gmatch(str, "[^%s]+") do
        v[index] = tonumber(num)
        index = index + 1
    end
    table.insert(state.vertices, v)
end

cmds.vn = function(state, str)
    local v = Vector()
    local index = 1
    for num in string.gmatch(str, "[^%s]+") do
        v[index] = tonumber(num)
        index = index + 1
    end

    table.insert(state.normals, v)
end

cmds.vt = function(state, str)
    local v = Vector()
    local index = 1
    for num in string.gmatch(str, "[^%s]+") do
        v[index] = tonumber(num)
        index = index + 1
    end

    table.insert(state.texCos, v)
end

cmds.f = function(state, str)
    if state.currentBinding == nil then
        state.currentBinding = {
            material = materials.default,
            triangles = {}
        }
        table.insert(state.bindings, state.currentBinding)
    end
    local shape = {}
    for split in string.gmatch(str, "[^%s]+") do
        local indices = {}
        local i = 1
        for index in string.gmatch(split, "[^/]*") do
            if #index ~= 0 then
                indices[i] = tonumber(index)
                i = i + 1
            end
        end
        table.insert(shape, indices)
    end

    -- Convert the shape into triangles.
    if #shape == 3 then
        local len = #state.currentBinding.triangles
        state.currentBinding.triangles[len + 1] = shape[1]
        state.currentBinding.triangles[len + 2] = shape[2]
        state.currentBinding.triangles[len + 3] = shape[3]
    elseif #shape == 4 then
        local len = #state.currentBinding.triangles
        state.currentBinding.triangles[len + 1] = shape[1]
        state.currentBinding.triangles[len + 2] = shape[2]
        state.currentBinding.triangles[len + 3] = shape[3]
        state.currentBinding.triangles[len + 4] = shape[3]
        state.currentBinding.triangles[len + 5] = shape[4]
        state.currentBinding.triangles[len + 6] = shape[1]
    end
end

cmds.usemtl = function(state, str)
    -- Create a new binding with the given material.
    state.currentBinding = {
        material = materials[str],
        triangles = {}
    }
    if not materials[str] then
        print("warning: unknown material from OBJ: " .. str)
        state.currentBinding.material = materials.default
    end
    table.insert(state.bindings, state.currentBinding)
end

function obj.parse(str)
    local state = {
        bindings = {},
        vertices = {},
        texCos = {},
        normals = {},
    }
    for line in string.gmatch(str, "[^\r\n]*") do
        command, remaining = string.match(
            line,
            "([^%s]+)%s*(.*)")
        if cmds[command] then
            cmds[command](state, remaining)
        end
    end
    
    local pointCloudPosition, pointCloudTexCo, pointCloudNormal
    if state.vertices[1] then
        pointCloudPosition = VertexArray:new {} end
    if state.texCos[1] then
        pointCloudTexCo = VertexArray:new {} end
    if state.normals[1] then
        pointCloudNormal = VertexArray:new {} end
    local mesh = Mesh:new {
        pointCloud = PointCloud:new {
            vertex = pointCloudPosition,
            texCo = pointCloudTexCo,
            normal = pointCloudNormal
        },
        bindings = {}
    }
    local indexTable = {}
    local currentIndex = 1
    -- Create a mesh from the state.
    for k, binding in ipairs(state.bindings) do
        mesh.bindings[k] = {
            material = binding.material,
            indexArray = IndexArray:new {}
        }
        for i, indices in ipairs(binding.triangles) do
            local t = indexTable
            for j, index in ipairs(indices) do
                if t[index] then
                    t = t[index]
                else
                    t[index] = {}
                    t = t[index]
                end
            end

            local index
            -- Insert the vertex if it is not in the table already.
            if not t[1] then
                t[1] = currentIndex
                if indices[1] then
                    mesh.pointCloud.vertex[currentIndex] =
                        state.vertices[indices[1]]
                end
                if indices[2] then
                    if indices[3] then
                        mesh.pointCloud.texCo[currentIndex] =
                            state.texCos[indices[2]]
                        mesh.pointCloud.normal[currentIndex] =
                            state.normals[indices[3]]
                    else
                        mesh.pointCloud.normal[currentIndex] =
                            state.normals[indices[2]]
                    end
                end
                index = currentIndex
                currentIndex = currentIndex + 1
            else
                index = t[1]
            end
            table.insert(mesh.bindings[k].indexArray, index)
        end
    end
    return mesh
end

function obj.loadObj(path)
    local f = io.open(path, "r")

    if not f then
        error("failed to open file.")
    end
    local str, e = f:read("a")
    f:close()

    if not str then
        error("failed to load OBJ: " .. e)
    end

    return obj.parse(str)
end
return obj
