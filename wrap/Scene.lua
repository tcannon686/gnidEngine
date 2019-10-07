
local Octree = require("octree/Octree")

local Scene = {}

function Scene.new(self, attributes)
    if type(attributes.activeCamera) ~= "table" then
        error("Scene.new expects table with key activeCamera.")
    end
    if type(attributes.objects) ~= "table" then
        error("Scene.new expects table with key objects.")
    end

    local objects = {
        __objects = {},
        __octrees = {}
    }
    for k, object in pairs(attributes.objects) do
        local octree = Octree:toOctree(object)
        if octree then
            objects.__octrees[tostring(octree)] = octree
        else
            objects.__objects[tostring(object)] = object
        end
    end
    local objectsMt = {}
    objectsMt.__pairs = function(t, k)
        return function(t, k)
            if k == nil then
                return next(rawget(t, "__objects"))
            else
                local objects
                local octrees
                
                objects = rawget(t, "__objects")
                octrees = rawget(t, "__octrees")

                if objects[k] then
                    local val, nextKey = next(objects, k)
                    if val == nil then
                        return next(octrees)
                    else
                        return val, nextKey
                    end
                else
                    return next(octrees, k)
                end
            end
        end, objects, nil
    end
    objectsMt.__newindex = function(t, k)
        error("Scene object.objects cannot be modified directly.")
    end
    setmetatable(objects, objectsMt)

    local ret = {
        objects = objects,
        activeCamera = attributes.activeCamera
    }
    
    ret.add = function(self, object)
        if type(object) ~= "table" then
            error("Scene.add expects object to add.")
        end
        local octree = Octree:toOctree(object)
        if octree then
            rawget(self.objects, "__octrees")[tostring(octree)] = octree
        else
            rawget(self.objects, "__objects")[tostring(object)] = object
        end
    end

    ret.remove = function(self, object)
        if type(object) ~= "table" then
            error("Scene.remove expects object to remove.")
        end
        local name = tostring(object)
        rawget(self.objects, "__objects")[name] = nil
        rawget(self.objects, "__octrees")[name] = nil
    end

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
