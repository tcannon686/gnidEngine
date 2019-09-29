
local VertexArray = {}
VertexArray.__mt = {}

function VertexArray.new(self, vertices)
    local ret = {}
    ret.__vertices = {}
    ret.__buffer = gl.buffer.new()
    ret.__capacity = 0
    ret.__needsUpdate = true
    
    -- Convert the vertices to vectors.
    for i, vertex in ipairs(vertices) do
        ret.__vertices[i] = Vector.toVector(vertex)
        if not ret.__vertices[i] then
            return error("VertexArray.new expects list of vectors.")
        end
    end

    ret.pack = function(self)
        return Vector.pack4array(self.__vertices)
    end

    ret.update = function(self)
        if self.__needsUpdate then
            gl.buffer.array.bind(self.__buffer)
            data = self:pack()
            len = #data
            -- Allocate more space if necessary.
            if self.__capacity < len then
                if self.__capacity == 0 then
                    self.__capacity = 4
                end
                while self.__capacity < len do
                    self.__capacity = self.__capacity * 2
                end
                gl.buffer.array.data(nil, self.__capacity)
            end

            gl.buffer.array.subData(data, 0)
            self.__needsUpdate = false
        end
    end

    ret.bind = function(self, index)
        gl.buffer.array.bind(self.__buffer)
        self:update()
        if index then
            gl.vertexAttrib.pointer(index, 4, 'f', false, 0, 0)
        end
    end

    ret.clear = function(self)
        if self.__vertices then
            for i, v in ipairs(self.__vertices) do
                self.__vertices[i] = nil
            end
        end
    end

    setmetatable(ret, self.__mt)
    ret:update()
    return ret
end

function VertexArray.toVertexArray(self, object)
    if getmetatable(object) == self.__mt then
        return object
    else
        return nil
    end
end

function VertexArray.__mt.__len(self)
    return #self.__vertices
end

function VertexArray.__mt.__index(self, k)
    if type(k) == "number" then
        return self.__vertices[k]
    end

    return rawget(self, k)
end


function VertexArray.__mt.__newindex(self, k, v)
    if type(k) == "number" then
        v = Vector.toVector(v)
        if v then
            self.__vertices[k] = v
            self.__needsUpdate = true
            return self
        else
            return error("VertexArray.__mt.__newindex expects vector.")
        end
    end

    return rawset(self, k, v)
end
return VertexArray
