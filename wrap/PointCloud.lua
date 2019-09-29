
local VertexArray = require("wrap/VertexArray")
local ShaderProgram = require("wrap/ShaderProgram")

local PointCloud = {}
PointCloud.__mt = {}

function PointCloud.__mt.__len(self)
    local ret = 0

    for array in pairs(self.vertexArrays) do
        local len = #array
        if len > ret then
            ret = len
        end
    end
    return ret
end

function PointCloud.__mt.__index(self, k)
    va = self.vertexArrays[k]
    if va then
        return va
    else
        return rawget(self, k)
    end
end

function PointCloud.__mt.__newindex(self, k, v)
    va = self.vertexArrays[v]
    if va then
        error("'" .. v .. "' is immutable.")
    end

    return rawset(self, k, v)
end

function PointCloud.new(self, attributes)
    local ret = {}

    ret.vertexArrays = {}
    if attributes then
        for key, value in pairs(attributes) do
            va = VertexArray:toVertexArray(value)
            if va == nil then
                error("PointCloud.new expects vertex arrays as attributes.")
            end
            ret.vertexArrays[key] = va
        end
    end

    ret.bind = function(self, shader)
        s = ShaderProgram:toShaderProgram(shader)
        if s == nil then
            error("PointCloud.bind expects a shader program.")
        end

        for key, value in pairs(self.vertexArrays) do
            s[key] = value
        end
    end

    ret.clear = function(self)
        for key, array in pairs(self.vertexArrays) do
            array:clear()
        end
    end

    ret.remap = function(self, a, b)
        self.vertexArrays[b] = self.vertexArrays[a]
        self.vertexArrays[a] = nil
    end

    setmetatable(ret, PointCloud.__mt)
    return ret
end

function PointCloud.toPointCloud(self, v)
    if getmetatable(v) == self.__mt then
        return v
    else
        return nil
    end
end

return PointCloud
