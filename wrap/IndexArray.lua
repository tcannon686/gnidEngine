--[[IndexArray for indexed triangles. Note that this object uses Lua's indexing
    starting from 1, not from 0.]]
local IndexArray = {}
IndexArray.__mt = {}

function IndexArray.__mt.__index(self, k, v)
    if type(k) == "number" then
        return self.__elements[k]
    end

    return rawget(self, k)
end

function IndexArray.__mt.__newindex(self, k, v)
    if type(k) == "number" then
        if type(v) == "number" then
            self.__elements[k] = v
            self.__needsUpdate = true
            return self
        else
            return error("IndexArray.__mt.__newindex expects number.")
        end
    end

    return rawset(self, k, v)
end

function IndexArray.__mt.__len(self)
    return #self.__elements
end

function IndexArray.new(self, elements)
    local ret = {}
    ret.__elements = {}
    ret.__buffer = gl.buffer.new()
    ret.__capacity = 0
    ret.__needsUpdate = true

    for i, index in ipairs(elements) do
        if type(index) == "number" then
            ret.__elements[i] = index
        else
            return error("IndexArray.new expects list of integers.")
        end
    end

    ret.pack = function(self, start, finish)
        local data = ''
        if not start then start = 1 end
        if not finish then finish = #self.__elements end

        for i = start, finish do
            data = data .. string.pack(
                "H",
                self.__elements[i] - 1)
        end
        return data
    end

    ret.bind = function(self)
        gl.buffer.element.bind(self.__buffer)
        self:update()
    end

    ret.update = function(self)
        if self.__needsUpdate then
            gl.buffer.element.bind(self.__buffer)
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
                gl.buffer.element.data(nil, self.__capacity)
            end

            gl.buffer.element.subData(data, 0)
            self.__needsUpdate = false
        end
    end

    ret.clear = function(self)
        if self.__elements then
            for i, v in ipairs(self.__elements) do
                self.__elements[i] = nil
            end
        end
    end

    ret.render = function(self)
        self:bind()
        gl.triangles.drawElements(#self, "H")
    end
    
    setmetatable(ret, self.__mt)
    ret:update()
    return ret
end

function IndexArray.toIndexArray(self, object)
    if getmetatable(object) == self.__mt then
        return object
    else
        return nil
    end
end


return IndexArray
