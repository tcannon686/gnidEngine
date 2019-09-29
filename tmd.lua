local Mesh = require("wrap/Mesh")
local Bone = require("wrap/Bone")
local Skeleton = require("wrap/Skeleton")
local SampledAnimationCurve = require("wrap/SampledAnimationCurve")
local Action = require("wrap/Action")
local materials = require("materials")
local tmd = {}

local levels = {
    WARNING = 1,
    ERROR = 2,
}
local version = {
    major = 1,
    minor = 1
}
local magicNumber = 0xCA770770
local recordTypes = {
    done = 0,
    version = 0x01,
    -- Vertices
    vertexAttrib = 0x10,
    vertex = 0x11,
    -- Bindings
    newBinding = 0x20,
    triangle = 0x21,
    -- Skeleton
    newBone = 0x30,
    parentBone = 0x31,
    nameBone = 0x32,
    -- Animation
    newAction = 0x40,
    newCurve = 0x41,
    curveSample = 0x42,
    playbackData = 0x43
}

local cmds = {}

function unpackNext(state, format)
    local packSize = string.packsize(format)
    state.index = state.index + packSize
    return string.unpack(format, state.str, state.index - packSize)
end

function unpackString(state)
    local str = string.unpack("<z", state.str, state.index)
    state.index = state.index + #str + 1
    return str
end

cmds[recordTypes.done] = function(state)
    state.hasMore = false
end

cmds[recordTypes.version] = function(state, blockType)
    local num
    state.version = {}
    num, state.version.major, state.version.minor = unpackNext(state, "<IBB")
    if num ~= magicNumber then
        return levels.ERROR, "not a tmd file"
    end
    if state.version.major ~= version.major then
        return levels.ERROR,
            "major version mismatch: " .. state.version.major ..  version.major
    end
end

cmds[recordTypes.vertexAttrib] = function(state, blockType)
    local name = unpackString(state)
    if not state.vertexAttribs[name] then
        state.vertexAttribs[name] = VertexArray:new {}
    end
    state.currentVertexAttrib = state.vertexAttribs[name]
    state.vertexIndex = #state.currentVertexAttrib + 1
end

cmds[recordTypes.vertex] = function(state, blockType)
    local x, y, z, w = unpackNext(state, "<ffff")
    if not state.currentVertexAttrib then
        return levels.ERROR, "attemped to create vertex without vertex array"
    end
    state.currentVertexAttrib[state.vertexIndex] = Vector(x, y, z, w)
    state.vertexIndex = state.vertexIndex + 1
end

cmds[recordTypes.newBinding] = function(state, blockType)
    local matName = unpackString(state)
    local material = materials[matName]

    if not material then
        return levels.WARNING, "unknown material \"" .. matName .. "\""
    end

    state.currentBinding = {
        material = material,
        indexArray = IndexArray:new {}
    }

    table.insert(state.bindings, state.currentBinding)
    state.indexIndex = 1
end

cmds[recordTypes.triangle] = function(state, blockType)
    local a, b, c = unpackNext(state, "<III")
    if state.currentBinding == nil then
        return levels.ERROR, "attempted to create index without binding"
    end
    state.currentBinding.indexArray[state.indexIndex] = a
    state.indexIndex = state.indexIndex + 1
    state.currentBinding.indexArray[state.indexIndex] = b
    state.indexIndex = state.indexIndex + 1
    state.currentBinding.indexArray[state.indexIndex] = c
    state.indexIndex = state.indexIndex + 1
end

cmds[recordTypes.newBone] = function(state, blockType)
    local length,
        a, b, c, d,
        e, f, g, h,
        i, j, k, l,
        m, n, o, p =
        unpackNext(state, "<fffffffffffffffff")

    local bone = Bone:new {
        length = length,
        matrix = Matrix(a, b, c, d,
                        e, f, g, h,
                        i, j, k, l,
                        m, n, o, p)
    }
    state.bones[state.boneIndex] = bone
    state.boneIndex = state.boneIndex + 1
end

cmds[recordTypes.parentBone] = function(state, blockType)
    local childIndex, parentIndex = unpackNext(state, "<II")
    if not state.bones[parentIndex].children then
        state.bones[parentIndex].children = {}
    end
    table.insert(state.bones[parentIndex].children, state.bones[childIndex])
    state.bones[childIndex].parent = state.bones[parentIndex]
end

cmds[recordTypes.nameBone] = function(state, blockType)
    local index, name
    index = unpackNext(state, "<I")
    name = unpackString(state)
    if state.namedBones[name] then
        state.namedBones[name] = state.bones[index]
        return levels.WARNING, "duplicate bone name \"" .. name .. "\""
    else
        state.namedBones[name] = state.bones[index]
    end
end

cmds[recordTypes.newAction] = function(state, blockType)
    local name, sampleCount

    name = unpackString(state)
    sampleCount = unpackNext(state, "<I")

    state.currentAction = {
        sampleCount = sampleCount,
        curves = {},
    }
    local overwrite = state.actions[name]
    state.actions[name] = state.currentAction
    state.curveIndex = 1
    if overwrite then
        return levels.WARNING, "action \"" .. name .. "\" overwritten"
    end
end

cmds[recordTypes.newCurve] = function(state, blockType)
    local boneIndex = unpackNext(state, "<B")
    if state.currentAction == nil then
        return levels.ERROR, "tried to create curve without an action"
    end
    state.currentCurve = {
        boneIndex = boneIndex
    }
    state.currentAction.curves[state.curveIndex]
        = state.currentCurve
    state.curveIndex = state.curveIndex + 1
    state.sampleIndex = 1
end

cmds[recordTypes.curveSample] = function(state, blockType)
    local a, b, c, d,
        e, f, g, h,
        i, j, k, l,
        m, n, o, p =
        unpackNext(state, "<ffffffffffffffff")
    local matrix = Matrix(a, b, c, d,
                          e, f, g, h,
                          i, j, k, l,
                          m, n, o, p)
    if state.currentCurve == nil then
        return levels.ERROR, "tried to add curve sample without curve"
    end
    if state.sampleIndex > state.currentAction.sampleCount then
        return levels.ERROR, "number of samples in curve greater than "
                             .. "number of samples in action"
    end
    state.currentCurve[state.sampleIndex] = matrix
    state.sampleIndex = state.sampleIndex + 1
end

cmds[recordTypes.playbackData] = function(state, blockType)
    local sampleRate = unpackNext(state, "<B")
    state.sampleRate = sampleRate
end

function parseRecord(state)
    local blockType = unpackNext(state, "<B")
    local cmd = cmds[blockType]
    if cmd ~= nil then
        local level, e = cmd(state, blockType)
        if level then
            return level, e
        end
    else
        return levels.ERROR, string.format("unknown record type: %x", blockType)
    end
end

-- Returns Mesh, Skeleton if successful, otherwise nil, string if failed.
function tmd.parse(str)
    local state = {
        vertexAttribs = {},
        bindings = {},
        bones = {},
        namedBones = {},
        actions = {},
        index = 1,
        vertexIndex = 1,
        indexIndex = 1,
        boneIndex = 1,
        hasMore = true,
        sampleRate = 24,
        str = str
    }
    while state.hasMore do
        local level, e = parseRecord(state)
        if level == levels.ERROR then
            return nil, e
        elseif level == levels.WARNING then
            print("warning: tmd: " .. e)
        end
    end

    local skeleton = nil

    if state.bones[1] then
        local isEmpty = true
        for k, v in pairs(state.namedBones) do
            isEmpty = false
            break
        end
        -- Include named bones if needed.
        if isEmpty then
            skeleton = Skeleton:new {
                bones = state.bones
            }
        else
            skeleton = Skeleton:new {
                bones = state.bones,
                namedBones = state.namedBones
            }
        end
    end

    local actions = nil
    for k, v in pairs(state.actions) do
        if not actions then
            actions = {} end
        local curves = {}
        for i, curve in ipairs(v.curves) do
            local animCurve = SampledAnimationCurve:new {
                samples = curve,
                sampleRate = state.sampleRate,
                lerp = true,
                dataPath = { "bones", curve.boneIndex, "matrix" }
            }
            curves[i] = animCurve
        end
        actions[k] = Action:new {
            curves = curves,
            loopEnd = v.sampleCount / state.sampleRate,
            loopStart = 0
        }
    end

    -- Create the mesh and skeleton from the data.
    return Mesh:new {
        pointCloud = PointCloud:new(state.vertexAttribs),
        bindings = state.bindings
    }, skeleton, actions
end

-- Returns Mesh, Skeleton if successful, otherwise nil, string if failed.
function tmd.loadTmd(path)
    local f = io.open(path, "rb")

    if not f then
        error("failed to open file.")
    end
    local str, e = f:read("a")
    f:close()

    if not str then
        error("failed to load tmd: " .. e)
    end

    return tmd.parse(str)
end

return tmd
