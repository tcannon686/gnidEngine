
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
            object:render(self)
        end
    end

    return ret
end

return Scene
