
local Scene = {}

function Scene.new(self, attributes)
    if type(attributes.activeCamera) ~= "table" then
        error("Scene.new expects table with key activeCamera.")
    end
    if type(attributes.objects) ~= "table" then
        error("Scene.new expects table with key objects.")
    end

    local ret = {
        objects = attributes.objects,
        activeCamera = attributes.activeCamera
    }

    ret.render = function(self)
        for k, object in pairs(self.objects) do
            if object.render then
                object:render(self)
            end
        end
    end

    ret.preTick = function(self, deltaT)
        for k, object in pairs(self.objects) do
            if object.preTick then
                object:preTick(deltaT, self)
            end
        end
    end

    ret.tick = function(self, deltaT)
        for k, object in pairs(self.objects) do
            if object.tick then
                object:tick(deltaT, self)
            end
        end
    end

    ret.postTick = function(self, deltaT)
        for k, object in pairs(self.objects) do
            if object.postTick then
                object:postTick(deltaT, self)
            end
        end
    end

    return ret
end

return Scene
