local Transform = {}
Transform.__mt = {}

function Transform.__mt.__index(self, k, v)
    if k == "worldMatrix" then
        return self.parent.worldMatrix * self.matrix
    end
    return rawget(self, k, v)
end

function Transform.__mt.__newindex(self, k, v)
    if k == "parent" then
        if self.parent then
            -- Remove this transform from its parent's children.
            local rmi = 0
            for i, child in self.parent.children do
                if child == self then
                    rmi = i
                    break
                end
            end

            if rmi then
                table.remove(self.parent.children, rmi)
            end
        end
        self.parent = v
        table.insert(self.parent.children, self)
    else
        rawset(self, k, v)
    end
end

function Transform.new(self, parent, matrix)
    local ret = {}
    setmetatable(ret, self.__mt)

    if parent then
        parent = Transform:toTransform(parent)
        if parent then
            ret.parent = parent
            table.insert(ret.parent.children, ret)
        else
            error("Transform.new expects parent Transform or nil.")
        end
    end

    if matrix then
        m = Matrix.toMatrix(matrix)
        if m then
            ret.matrix = m
        else
            error("Transform.new expects a Matrix or nil.")
        end
    else
        ret.matrix = Matrix.identity()
    end
end

function Transform.toTransform(self, object)
    if getmetatable(object) == self.__mt then
        return object
    else
        return nil
    end
end

return Transform
