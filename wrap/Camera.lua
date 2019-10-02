local Camera = {}
Camera.__mt = {}

function Camera.__mt.__newindex(t, k, v)
    if k == "matrix" then
        t.__matrix = v
        t.viewMatrix = t.matrix:inverse()
    else
        rawset(t, k, v)
    end
end

function Camera.__mt.__index(t, k, v)
    if k == "matrix" then
        return t.__matrix
    end
end

function Camera.new(self, attributes)
    local ret = {}
    if attributes.matrix then
        ret.__matrix = attributes.matrix
        ret.viewMatrix = ret.matrix:inverse()
    else
        ret.__matrix = Matrix.identity
        ret.viewMatrix = Matrix.identity
    end

    if not attributes.projection then
        ret.projection = Matrix.newPerspective(
            90 * math.pi / 180,
            window.width / window.height, 
            0.1,
            100.0)
    end

    ret.render = function(self, scene)
        -- pass
    end

    setmetatable(ret, self.__mt)

    return ret
end

return Camera
