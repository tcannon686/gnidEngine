
local Octree = require("octree/Octree")

local Scene = {}

local shaders = require("shaders")

function Scene.new(self, attributes)
    if type(attributes.activeCamera) ~= "table" then
        error("Scene.new expects table with key activeCamera.")
    end
    if type(attributes.objects) ~= "table" then
        error("Scene.new expects table with key objects.")
    end

    local objectListMt = {}
    objectListMt.__pairs = function(t, k)
        return next, rawget(t, "__data")
    end
    objectListMt.__newindex = function(t, k)
        error("Scene objects list cannot be modified directly.")
    end
    objectListMt.__len = function(t, k)
        local count = 0
        for t, v in pairs(t, "__data") do
            count = count + 1
        end
        return count
    end

    objects = { __data = {} }
    lights = { __data = {} }
    for k, object in pairs(attributes.objects) do
        objects.__data[tostring(object)] = object
        if object.light then
            lights.__data[tostring(object)] = object
        end
    end
    setmetatable(objects, objectListMt)
    setmetatable(lights, objectListMt)

    local ret = {
        objects = objects,
        lights = lights,
        activeCamera = attributes.activeCamera
    }
    
    ret.add = function(self, object)
        if type(object) ~= "table" then
            error("Scene.add expects object to add.")
        end
        rawget(self.objects, "__data")[tostring(object)] = object
        if object.light then
            rawget(self.lights, "__data")[tostring(object)] = object
        end
    end

    ret.remove = function(self, object)
        if type(object) ~= "table" then
            error("Scene.remove expects object to remove.")
        end
        local name = tostring(object)
        rawget(self.objects, "__data")[name] = nil
        rawget(self.lights, "__data")[name] = nil
    end

    ret.render = function(self)
        -- Update the lights for each shader.
        for name, shader in pairs(shaders) do
            if shader.lightCount ~= nil then
                shader.lightCount = #self.lights
                lightIndex = 1
                for k, object in pairs(self.lights) do
                    local position = 
                        self.activeCamera.viewMatrix
                            * object.position:homo()
                    shader.lightPositionAndDistance[lightIndex] =
                        position:three()
                            + Vector.fourth * object.light.distance
                    lightIndex = lightIndex + 1
                end
            end
        end
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

    ret.raycast = function(self, hit, ray)
        result = false
        for k, object in pairs(self.objects) do
            if object.raycast then
                if object:raycast(hit, ray) then
                    result = true
                end
            end
        end
        return result
    end

    return ret
end

return Scene
