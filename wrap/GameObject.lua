
local Transform = require("wrap/Component")

local GameObject = {}
GameObject.__mt = {}

function GameObject.new(self, parent)
    local ret = {}

    ret.__components = {}

    ret.transform = Transform:new(parent)
    ret.transform.gameObject = ret

    ret.getComponents = function(self, component)    
        local t = {}
        for i, v in ipairs(self.__components[component.id]) do
            table.insert(t, v)
        end
        return t
    end

    ret.getComponent = function(self, component)
        local t = self.__components[component.id]
        if t then
            return t[1]
        else
            return nil
        end
    end

    ret.addComponent = function(self, component)
        local t = self.__components[component.id]
        table.insert(t, component)
    end

    ret.start = function(self)
        for name, component in pairs(self.components) do
            if component.start then
                component:start()
            end
        end
    end

    ret.awake = function(self)
        for name, component in pairs(self.components) do
            if component.awake then
                component:awake()
            end
        end
    end

    ret.update = function(self)
        for name, component in pairs(self.components) do
            if component.update then
                component:update()
            end
        end
    end

    ret.render = function(self)
        for name, component in pairs(self.components) do
            if component.render then
                component:render()
            end
        end
    end
end

return GameObject
